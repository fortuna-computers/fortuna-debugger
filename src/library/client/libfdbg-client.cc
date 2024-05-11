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
#include <string>
#include <thread>

extern "C" {
#include "../common/terminal.h"
}

using namespace std::chrono_literals;
using namespace std::string_literals;
using hrc = std::chrono::high_resolution_clock;

void FdbgClient::connect(std::string const& port, uint32_t baudrate)
{
    fd_ = open(port.c_str(), O_RDWR);
    if (fd_ < 0)
        throw std::runtime_error("Could not open serial port "s + port + ": " + strerror(errno));

    configure_terminal_settings(fd_, baudrate);
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

fdbg::ToDebugger FdbgClient::send_message(fdbg::ToComputer const& msg, bool check_for_errors)
{
    send(msg);
    return receive(check_for_errors);
}

void FdbgClient::send(fdbg::ToComputer const& msg)
{
    std::string message;
    msg.SerializeToString(&message);

    uint8_t sz = (uint8_t) message.length();

    if (message.size() > MAX_MESSAGE_SZ)
        throw std::runtime_error("Message too large: "s + msg.DebugString());

    if (debugging_level_ != DebuggingLevel::NORMAL) {
        printf("-> %s", msg.DebugString().c_str());
        if (debugging_level_ == DebuggingLevel::TRACE) {
            printf("=> [%02hhX]", sz);
            for (char c: message)
                printf(" %02hhX", (uint8_t) c);
            printf("\n");
        }
    }

    ssize_t r = write(fd_, &sz, 1);
    if (r < 0)
        throw std::runtime_error("Error writing to serial.");
    r = write(fd_, message.data(), message.length());
    if (r < 0)
        throw std::runtime_error("Error writing to serial.");
}

fdbg::ToDebugger FdbgClient::receive(bool check_for_errors)
{
start:
    uint8_t sz;
    ssize_t r = read(fd_, &sz, 1);
    if (r == 0) {
        std::this_thread::sleep_for(10us);
        goto start;
    } else if (r == -1 && errno != EAGAIN) {
        throw std::runtime_error("Error reading message size from serial: "s + strerror(errno));
    }

    if (debugging_level_ == DebuggingLevel::TRACE)
        printf("<= [%02hhX]", sz);

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

    if (check_for_errors && msg.status() != fdbg::Status::OK)
        throw std::runtime_error("Operation failed with error " + std::to_string(msg.status()));

    return msg;
}

void FdbgClient::ack(uint32_t id)
{
    fdbg::ToComputer msg;
    msg.set_allocated_ack(new fdbg::Ack());

    auto response = send_message(msg, fdbg::ToDebugger::kAckResponse);

    if (response.ack_response().id() != id)
        throw std::runtime_error("Invalid id received from server during ack");
}

void FdbgClient::write_memory(uint64_t pos, std::vector<uint8_t> const& data, bool validate)
{
    if (data.size() > MAX_MEMORY_TRANSFER)
        throw std::runtime_error("Cannot write more than " + std::to_string(MAX_MEMORY_TRANSFER) + " bytes at time.");

    // request
    auto write_memory = new fdbg::WriteMemory();
    write_memory->set_initial_addr(pos);
    write_memory->set_bytes(data.data(), data.size());
    write_memory->set_validate(validate);

    fdbg::ToComputer msg;
    msg.set_allocated_write_memory(write_memory);

    auto response = send_message(msg, false);

    if (response.status() != fdbg::Status::OK)
        throw std::runtime_error(std::format("Error writing memory: first byte failed is 0x{:x}", response.write_memory_response().first_failed_pos()));
}

std::vector<uint8_t> FdbgClient::read_memory(uint64_t pos, uint8_t sz, uint8_t sequences)
{
    if (sz > MAX_MEMORY_TRANSFER)
        throw std::runtime_error("Cannot read more than " + std::to_string(MAX_MEMORY_TRANSFER) + " bytes at time.");

    auto read_memory = new fdbg::ReadMemory();
    read_memory->set_initial_addr(pos);
    read_memory->set_sz(sz);
    read_memory->set_sequences(sequences);

    fdbg::ToComputer msg;
    msg.set_allocated_read_memory(read_memory);

    auto response = send_message(msg, fdbg::ToDebugger::kReadMemoryResponse);

    return { response.read_memory_response().bytes().begin(), response.read_memory_response().bytes().end() };
}

