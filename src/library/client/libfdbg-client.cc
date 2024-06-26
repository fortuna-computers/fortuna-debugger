#include "libfdbg-client.hh"

#include <cerrno>
#include <cstdio>
#include <fcntl.h>

#if __APPLE__
#  include <util.h>
#else
#  include <pty.h>
#  include <stdio.h>
#  include <termios.h>
#endif
#include <unistd.h>

#include <chrono>
#include <format>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>

using namespace std::chrono_literals;
using namespace std::string_literals;
using hrc = std::chrono::high_resolution_clock;

void FdbgClient::load_user_definition(std::string const& filename)
{
    machine_.load_user_definition(filename);
}

static int configure_terminal_settings(int fd, uint32_t baud)
{
    struct termios tio;
    tcgetattr(fd, &tio);
    cfmakeraw(&tio);
    cfsetspeed(&tio, baud);
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 0;
    if (tcsetattr(fd, TCSANOW, &tio) != 0)
        return -1;

    if (fcntl(fd, F_SETFL, FNDELAY) != 0)
        return -1;

    return 0;
}

void FdbgClient::connect(std::string const& port, uint32_t baudrate)
{
    fd_ = open(port.c_str(), O_RDWR);
    if (fd_ < 0)
        throw std::runtime_error("Could not open serial port "s + port + ": " + strerror(errno));

    configure_terminal_settings(fd_, baudrate);

    std::this_thread::sleep_for(std::chrono::milliseconds(machine_.uc_wait));

    // clear queue
    char u;
    ssize_t r;
    do { r = read(fd_, &u, 1); } while (r == 1);
}

FdbgClient::~FdbgClient()
{
    if (fd_ != -1)
        close(fd_);
    google::protobuf::ShutdownProtobufLibrary();
}

std::string FdbgClient::autodetect_usb_serial_port(std::string const& vendor_id, std::string const& product_id)
{
    char port[512];

    FILE* fp = popen(("./findserial.py "s + vendor_id + " " + product_id).c_str(), "r");
    if (!fp)
        throw std::runtime_error("Could not find auto-detect script");

    fgets(port, sizeof(port), fp);
    if (pclose(fp) != 0)
        throw std::runtime_error("Auto-detect unsuccessful");

    return port;
}

fdbg::ToDebugger FdbgClient::send_message(fdbg::ToComputer const& msg, fdbg::ToDebugger::MessageCase message_type)
{
    send(msg);
    return receive(message_type);
}

void FdbgClient::send(fdbg::ToComputer const& msg)
{
    std::string message;
    msg.SerializeToString(&message);

    auto sz = message.length();

    if (message.size() > MAX_MESSAGE_SZ)
        throw std::runtime_error("Message too large: "s + msg.DebugString());

    uint8_t sz1, sz2;
    if (sz <= 0x7f) {
        sz1 = sz;
    } else {
        sz1 = ((sz & 0x7f) | 0x80);
        sz2 = (sz >> 7);
    }

    if (debugging_level_ != DebuggingLevel::NORMAL) {
        printf("-> %s", msg.DebugString().c_str());
        if (debugging_level_ == DebuggingLevel::TRACE) {
            if (sz <= 0x7f)
                printf("=> [%02X]", sz1);
            else
                printf("=> [%02X %02X]", sz1, sz2);
            for (char c: message)
                printf(" %02X", (uint8_t) c);
            printf("\n");
        }
    }

    ssize_t r = write(fd_, &sz1, 1);
    if (r < 0)
        throw std::runtime_error("Error writing to serial: "s + strerror(errno));
    if (sz > 0x7f)
        write(fd_, &sz2, 1);

    r = write(fd_, message.data(), message.length());
    if (r < 0)
        throw std::runtime_error("Error writing to serial.");
}

fdbg::ToDebugger FdbgClient::receive(fdbg::ToDebugger::MessageCase message_type)
{
start:
    // read size
    uint8_t sz1;
    auto now = hrc::now();
    ssize_t r = read(fd_, &sz1, 1);
    if (r == 0 || errno == EAGAIN) {
        std::this_thread::sleep_for(10us);
        if (hrc::now() > (now + 2s))
            throw std::runtime_error("Response not received in 2 seconds");
        goto start;
    } else if (r == -1 && errno != EAGAIN) {
        throw std::runtime_error("Error reading message size from serial: "s + strerror(errno));
    }

    uint16_t sz;
    uint8_t sz2;
    if (sz1 & (1 << 7)) {
        read(fd_, &sz2, 1);
        sz = ((uint16_t) sz2 << 7) | (sz1 & 0x7f);
    } else {
        sz = sz1;
    }

    if (debugging_level_ == DebuggingLevel::TRACE) {
        if (sz1 & (1 << 7))
            printf("<= [%02X %02X]", sz1, sz2);
        else
            printf("<= [%02X]", sz1);
    }

    // read message
    if (sz > MAX_MESSAGE_SZ)
        throw std::runtime_error("Message received too big (" + std::to_string(sz) + " )");

    uint8_t message[sz];

    auto start = hrc::now();
    int count = 0;
    do {
        if (hrc::now() > start + 3s) {
            throw std::runtime_error("Waiting for more than 3 seconds for a whole message - only received part of it.");
        }
        r = read(fd_, &message[count], sz);
        if (r == 0 || (r == -1 && errno == EAGAIN)) {
            std::this_thread::sleep_for(10us);
            continue;
        } else if (r == -1) {
            throw std::runtime_error("Error reading message from serial: "s + strerror(errno));
        }
        count += r;
    } while (count < sz);

    if (debugging_level_ == DebuggingLevel::TRACE) {
        for (uint8_t c: message)
            printf(" %02hhX", (uint8_t) c);
        printf("\n");
    }

    fdbg::ToDebugger msg;
    msg.ParseFromArray(message, sz);

    if (debugging_level_ != DebuggingLevel::NORMAL)
        printf("<- %s", msg.DebugString().c_str());

    switch (msg.status()) {
        case fdbg::OK:                      break;
        case fdbg::ERR_WRITING_MEMORY:      throw std::runtime_error("Error writing to memory");
        case fdbg::TOO_MANY_BREAKPOINTS:    throw std::runtime_error("Too many breakpoints");
        case fdbg::INVALID_MESSAGE:         throw std::runtime_error("Invalid message (message type does not exist or was sent at the wrong time)");
        case fdbg::IO_SERIAL_ERROR:         throw std::runtime_error("I/O serial error reported by the computer");
        case fdbg::CPU_INVALID_INSTRUCTION: throw std::runtime_error("Invalid CPU instruction.");
        case fdbg::METHOD_NOT_IMPLEMENTED:  throw std::runtime_error("Method not implemented.");
        case fdbg::INFINITE_LOOP:           throw std::runtime_error("Infinite loop in the machine.");
        case fdbg::MESSAGE_TOO_SHORT: {
            std::string err = "Message too short: received";
            for (uint8_t c: msg.error_details())
                err += std::format(" {:02X}", c);
            throw std::runtime_error(err);
        }
        default:                            throw std::runtime_error("Message with error code " + std::to_string(msg.status()) + " received from computer.");
    }

    if (msg.message_case() == fdbg::ToDebugger::kDebug) {
        if (on_debug_)
            on_debug_(msg.debug().text());
        return receive(message_type);
    }

    if (msg.message_case() != fdbg::ToDebugger::MESSAGE_NOT_SET && msg.message_case() != message_type)
        throw std::runtime_error("Invalid message received from computer (unexpected response type)");

    return msg;
}

fdbg::AckResponse FdbgClient::ack(uint32_t id)
{
    fdbg::ToComputer msg;
    msg.set_allocated_ack(new fdbg::Ack());

    auto response = send_message(msg, fdbg::ToDebugger::kAckResponse);

    if (response.ack_response().id() != id)
        throw std::runtime_error("Invalid id received from server during ack");

    return response.ack_response();
}

fdbg::ComputerStatus FdbgClient::reset()
{
    fdbg::ToComputer msg;
    msg.set_allocated_reset(new fdbg::Reset());

    return send_message(msg, fdbg::ToDebugger::kComputerStatus).computer_status();
}

fdbg::ComputerStatus FdbgClient::step(bool full, std::vector<fdbg::UserEvent> const& events)
{
    fdbg::ToComputer msg;
    auto step = new fdbg::Step();
    step->set_full(full);
    for (auto const& event: events)
        step->add_user_events()->CopyFrom(event);
    msg.set_allocated_step(step);

    return send_message(msg, fdbg::ToDebugger::kComputerStatus).computer_status();
}

fdbg::CycleResponse FdbgClient::cycle()
{
    fdbg::ToComputer msg;
    msg.set_allocated_cycle(new fdbg::Cycle());
    return send_message(msg, fdbg::ToDebugger::kCycleResponse).cycle_response();
}

void FdbgClient::run(bool forever)
{
    fdbg::ToComputer msg;
    auto run = new fdbg::Run();
    run->set_forever(forever);
    msg.set_allocated_run(run);
    send_message(msg, fdbg::ToDebugger::MESSAGE_NOT_SET);
}

fdbg::RunStatus FdbgClient::next()
{
    fdbg::ToComputer msg;
    msg.set_allocated_next(new fdbg::Next());
    return send_message(msg, fdbg::ToDebugger::kRunStatus).run_status();
}

fdbg::RunStatus FdbgClient::run_status(std::vector<fdbg::UserEvent> const& events)
{
    fdbg::ToComputer msg;
    auto get_run_status = new fdbg::GetRunStatus();
    for (auto const& event: events)
        get_run_status->add_user_events()->CopyFrom(event);
    msg.set_allocated_get_run_status(get_run_status);
    return send_message(msg, fdbg::ToDebugger::kRunStatus).run_status();
}

fdbg::ComputerStatus FdbgClient::pause()
{
    fdbg::ToComputer msg;
    msg.set_allocated_pause(new fdbg::Pause());
    return send_message(msg, fdbg::ToDebugger::kComputerStatus).computer_status();
}

void FdbgClient::write_memory(uint8_t nr, uint64_t pos, std::span<const uint8_t> const& data, bool validate)
{
    if (data.size() > MAX_MEMORY_TRANSFER)
        throw std::runtime_error("Cannot write more than " + std::to_string(MAX_MEMORY_TRANSFER) + " bytes at time.");

    // request
    auto write_memory = new fdbg::WriteMemory();
    write_memory->set_memory_nr(nr);
    write_memory->set_initial_addr(pos);
    write_memory->set_bytes(data.data(), data.size());
    write_memory->set_validate(validate);

    fdbg::ToComputer msg;
    msg.set_allocated_write_memory(write_memory);

    auto response = send_message(msg, fdbg::ToDebugger::kWriteMemoryResponse);

    if (response.status() != fdbg::Status::OK)
        throw std::runtime_error(std::format("Error writing memory: first byte failed is 0x{:x}", response.write_memory_response().first_failed_pos()));
}

std::vector<uint8_t> FdbgClient::read_memory(uint8_t nr, uint64_t pos, uint8_t sz, uint8_t sequences)
{
    if (sz > MAX_MEMORY_TRANSFER)
        throw std::runtime_error("Cannot read more than " + std::to_string(MAX_MEMORY_TRANSFER) + " bytes at time.");

    auto read_memory = new fdbg::ReadMemory();
    read_memory->set_memory_nr(nr);
    read_memory->set_initial_addr(pos);
    read_memory->set_sz(sz);
    read_memory->set_sequences(sequences);

    fdbg::ToComputer msg;
    msg.set_allocated_read_memory(read_memory);

    auto response = send_message(msg, fdbg::ToDebugger::kReadMemoryResponse);

    return { response.read_memory_response().bytes().begin(), response.read_memory_response().bytes().end() };
}

void FdbgClient::write_memory_full(uint8_t nr, uint64_t pos, std::span<const uint8_t> const& data, bool validate)
{
    Upload upload;
    while (write_memory_step(nr, pos, data, upload, validate))
        ;
}

bool FdbgClient::write_memory_step(uint8_t nr, uint64_t pos, std::span<const uint8_t> const& data, Upload& upload, bool validate)
{
    uint64_t initial_pos = pos + (upload.next * MAX_MEMORY_TRANSFER);

    if ((upload.next + 1) * MAX_MEMORY_TRANSFER > data.size()) {    // less than 64 bytes left
        std::span<const uint8_t> data_span(data.begin() + (upload.next * MAX_MEMORY_TRANSFER), data.end());
        write_memory(nr, initial_pos, data_span, validate);
        return false;

    } else {    // more than 64 bytes left
        std::span<const uint8_t> data_span(data.data() + initial_pos, MAX_MEMORY_TRANSFER);
        write_memory(nr, initial_pos, data_span, validate);
        ++upload.next;
        return true;
    }
}

void FdbgClient::compile_and_write_memory(std::string const& source_filename, uint8_t nr, bool validate)
{
    DebugInfo di = machine_.compile(source_filename);
    if (!di.success)
        throw std::runtime_error(di.error_info);
    for (auto const& bin: di.binaries)
        write_memory_full(bin.load_pos, nr, bin.rom, validate);
}

std::string FdbgClient::start_emulator(std::string const& path)
{
    // create pipe
    int pipefd[2];
    if (pipe(pipefd) != 0)
        throw std::runtime_error("Error creating pipe: "s + strerror(errno));

    // fork
    pid_t pid = fork();
    if (pid == -1)
        throw std::runtime_error("Error creating fork: "s + strerror(errno));

    if (pid == 0) {   // child process (server, emulator)
        // execute emulator
        execl(path.c_str(), path.c_str(), nullptr);
        throw std::runtime_error("Error executing emulator: "s + strerror(errno));

    } else {   // parent process (client)
        std::this_thread::sleep_for(400ms);
        return get_emulator_port(pid);
    }
}

std::string FdbgClient::get_emulator_port(pid_t pid)
{
    std::string filename = "/tmp/fdbg." + std::to_string(pid);
    std::ifstream f(filename);
    if (!f.good())
        throw std::runtime_error("Could not open file " + filename);
    std::stringstream buffer;
    buffer << f.rdbuf();
    return buffer.str();
}

std::set<uint64_t> FdbgClient::add_breakpoint(uint64_t addr)
{
    return breakpoint(fdbg::Breakpoint_Action_ADD, addr);
}

std::set<uint64_t> FdbgClient::remove_breakpoint(uint64_t addr)
{
    return breakpoint(fdbg::Breakpoint_Action_REMOVE, addr);
}

void FdbgClient::clear_breakpoints()
{
    breakpoint(fdbg::Breakpoint_Action_CLEAR_ALL, 0);
}

std::set<uint64_t> FdbgClient::breakpoint(fdbg::Breakpoint::Action action, uint64_t addr)
{
    fdbg::ToComputer msg;
    auto bkp = new fdbg::Breakpoint();
    bkp->set_action(action);
    bkp->set_address(addr);
    msg.set_allocated_breakpoint(bkp);

    auto rmsg = send_message(msg, fdbg::ToDebugger::kBreakpointList);
    if (rmsg.status() == fdbg::TOO_MANY_BREAKPOINTS)
        throw std::runtime_error("Too many breakpoints");

    std::set<uint64_t> ret;
    for (size_t i = 0; i < (size_t) rmsg.breakpoint_list().addr_size(); ++i)
        ret.insert(rmsg.breakpoint_list().addr(i));
    return ret;
}

void FdbgClient::interrupt(uint64_t number)
{
    fdbg::ToComputer msg;
    auto interrupt = new fdbg::Interrupt();
    interrupt->set_number(number);
    msg.set_allocated_interrupt(interrupt);

    send_message(msg, fdbg::ToDebugger::MESSAGE_NOT_SET);
}
