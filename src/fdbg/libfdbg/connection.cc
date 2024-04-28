#include "connection.hh"

#include <unistd.h>

#if __APPLE__
#  include <util.h>
#else
#  include <pty.h>
#  include <stdio.h>
#  include <termios.h>
#endif

#include <stdexcept>
#include <string>
using namespace std::string_literals;

Connection::Connection(char const *port, uint32_t baudrate)
{
    int slave_fd;

    char* serial_port = strdup(port);
    if (openpty(&master_fd_, &slave_fd, serial_port, nullptr, nullptr) == -1)
        throw std::runtime_error("Could not open serial port "s + serial_port);

    // configure terminal settings
    struct termios tio {};
    tcgetattr(master_fd_, &tio);
    cfmakeraw(&tio);
    tcsetattr(master_fd_, TCSANOW, &tio);
}

Connection::~Connection()
{
    close(master_fd_);
}

void Connection::send_request(Request const &request) const
{
    std::string message;
    request.SerializeToString(&message);

    uint8_t sz[2] = { static_cast<uint8_t>((message.length() >> 8) & 0xff), static_cast<uint8_t>(message.length() & 0xff) };

    if (message.size() > 0xffff)
        throw std::runtime_error("Message too large: "s + request.DebugString());

    if (debugging_level_ != DebuggingLevel::NORMAL) {
        printf("-> %s\n", request.DebugString().c_str());
        if (debugging_level_ == DebuggingLevel::TRACE) {
            printf("=> %02hhX %02hhX", sz[0], sz[1]);
            for (char c: message)
                printf(" %02hhX", (uint8_t) c);
            printf("\n");
        }
    }

    write(master_fd_, sz, 2);
    write(master_fd_, message.data(), message.length());
}

Response Connection::receive_response() const
{
    uint8_t sz[2];
    read(master_fd_, &sz, 2);

    if (debugging_level_ == DebuggingLevel::TRACE)
        printf("=> %02hhX %02hhX", sz[0], sz[1]);

    uint16_t tsz = sz[1] + ((uint16_t) sz[0] << 8);
    uint8_t message[tsz];
    read(master_fd_, message, tsz);

    if (debugging_level_ == DebuggingLevel::TRACE) {
        for (uint8_t c: message)
            printf(" %02hhX", (uint8_t) c);
        printf("\n");
    }

    Response response;
    response.ParseFromArray(message, tsz);

    if (debugging_level_ != DebuggingLevel::NORMAL)
        printf("-> %s\n", response.DebugString().c_str());

    return response;
}