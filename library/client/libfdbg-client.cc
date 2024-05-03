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

void FdbgClient::send_message(fdbg::ToComputer const &msg) const
{
    std::string message;
    msg.SerializeToString(&message);

    uint8_t sz[2] = { static_cast<uint8_t>((message.length() >> 8) & 0xff), static_cast<uint8_t>(message.length() & 0xff) };

    if (message.size() > MAX_MESSAGE_SZ)
        throw std::runtime_error("Message too large: "s + msg.DebugString());

    if (debugging_level_ != DebuggingLevel::NORMAL) {
        printf("-> %s", msg.DebugString().c_str());
        if (debugging_level_ == DebuggingLevel::TRACE) {
            printf("=> %02hhX %02hhX", sz[0], sz[1]);
            for (char c: message)
                printf(" %02hhX", (uint8_t) c);
            printf("\n");
        }
    }

    ssize_t r = write(fd_, sz, 2);
    if (r < 0)
        throw std::runtime_error("Error writing to serial.");
    r = write(fd_, message.data(), message.length());
    if (r < 0)
        throw std::runtime_error("Error writing to serial.");
}

std::optional<fdbg::ToDebugger> FdbgClient::receive_message() const
{
    uint8_t sz[2];
    ssize_t r = read(fd_, &sz, 2);
    if (r == 0)
        return {};
    else if (r == -1 && errno != EAGAIN)
        throw std::runtime_error("Error reading from serial: "s + strerror(errno));

    if (debugging_level_ == DebuggingLevel::TRACE)
        printf("<= %02hhX %02hhX", sz[0], sz[1]);

    uint16_t tsz = sz[1] + ((uint16_t) sz[0] << 8);
    uint8_t message[tsz];
    r = read(fd_, message, tsz);
    if (r < tsz)
        throw std::runtime_error("Error reading from serial (message smaller than expected)");

    if (debugging_level_ == DebuggingLevel::TRACE) {
        for (uint8_t c: message)
            printf(" %02hhX", (uint8_t) c);
        printf("\n");
    }

    fdbg::ToDebugger msg;
    msg.ParseFromArray(message, tsz);

    if (debugging_level_ != DebuggingLevel::NORMAL)
        printf("<- %s", msg.DebugString().c_str());

    return msg;
}

fdbg::ToDebugger FdbgClient::wait_for_response(std::function<bool(fdbg::ToDebugger const& msg)> check_function) const
{
    std::optional<fdbg::ToDebugger> response;

    auto initial_time = hrc::now();
    for (;;) {
        response = receive_message();
        if (!response)
            std::this_thread::sleep_for(1ms);
        else if (check_function(response.value()))
            return response.value();

        if (hrc::now() > initial_time + 2s)
            throw std::runtime_error("Timeout while waiting response from the server");
    }
}

void FdbgClient::ack_sync(uint32_t id) const
{
    fdbg::ToComputer msg;

    msg.set_allocated_ack(new fdbg::Ack());

    send_message(msg);

    auto response = wait_for_response([](fdbg::ToDebugger const& msg) { return msg.has_ack_response(); });
    if (response.ack_response().id() != id)
        throw std::runtime_error("Invalid id received from server during ack");
}