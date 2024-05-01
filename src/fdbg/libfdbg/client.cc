#include "client.hh"

#include <fcntl.h>
#include <unistd.h>

#if __APPLE__
#  include <util.h>
#else
#  include <pty.h>
#  include <stdio.h>
#  include <termios.h>
#endif

#include <chrono>
#include <stdexcept>
#include <string>
#include <thread>

#include "exceptions.hh"

using namespace std::chrono_literals;
using namespace std::string_literals;
using hrc = std::chrono::high_resolution_clock;

namespace fdbg {

void DebuggerClient::connect(std::string const& port, uint32_t baudrate)
{
    fd_ = open(port.c_str(), O_RDWR);
    if (fd_ < 0)
        throw std::runtime_error("Could not open serial port "s + port);

    configure_terminal_settings(baudrate);
}

ToDebugger DebuggerClient::wait_for_response(std::function<bool(ToDebugger const& msg)> check_function) const
{
    std::optional<ToDebugger> response;

    auto initial_time = hrc::now();
    for (;;) {
        response = receive();
        if (!response)
            std::this_thread::sleep_for(1ms);
        else if (check_function(response.value()))
            return response.value();

        if (hrc::now() > initial_time + 2s)
            throw Timeout();
    }
}

std::string DebuggerClient::start_emulator(std::string const& path)
{
    if (fork() == 0) {
        execl((path + "/f-emulator").c_str(), "f-emulator", nullptr);
        printf("Emulator killed!\n");
        abort();
    } else {
        std::this_thread::sleep_for(200ms);
        return read_port_from_emulator();
    }
}

std::string DebuggerClient::read_port_from_emulator() const
{
    std::string filename = std::string(getenv("TMPDIR")) + "/f-emulator";

    FILE* f = fopen(filename.c_str(), "r");
    char buffer[128];
    size_t i = 0;

    char c;
    do {
        c = fgetc(f);
        buffer[i++] = c;
    } while (c != EOF);
    fclose(f);

    buffer[i++] = 0;
    return buffer;
}

void DebuggerClient::ack_sync(uint32_t id) const
{
    ToComputer msg;

    msg.set_allocated_ack(new Ack());

    send(msg);

    auto response = wait_for_response([](ToDebugger const& msg) { return msg.has_ack_response(); });
    if (response.ack_response().id() != id)
        throw InvalidId();
}

void DebuggerClient::write_memory(uint64_t pos, std::vector<uint8_t> const& area) const
{
    ToComputer msg;

    auto write_memory = new WriteMemory();
    write_memory->set_initial_addr(pos);
    write_memory->set_bytes(area.data(), area.size());

    send(msg);
}

void DebuggerClient::write_memory_sync(uint64_t pos, std::vector<uint8_t> const& area) const
{
    write_memory(pos, area);

    auto response = wait_for_response([](ToDebugger const& msg) { return msg.has_write_memory_confirmation(); });
    if (response.write_memory_confirmation().error())
        throw WriteMemoryValidationError(response.write_memory_confirmation().first_failed_pos());
}

void DebuggerClient::read_memory_request(uint64_t pos, uint16_t sz) const
{
    ToComputer msg;

    auto read_memory = new ReadMemory();
    read_memory->set_initial_addr(pos);
    read_memory->set_sz(sz);
    msg.set_allocated_read_memory(read_memory);

    send(msg);
}

}
