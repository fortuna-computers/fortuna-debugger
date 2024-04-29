#include "serial.hh"

#include <cerrno>
#include <unistd.h>

#include <stdexcept>
#include <string>

using namespace std::string_literals;

namespace fdbg {

template <typename T>
void Serial::send_message(T const& msg) const
{
    std::string message;
    msg.SerializeToString(&message);

    uint8_t sz[2] = { static_cast<uint8_t>((message.length() >> 8) & 0xff), static_cast<uint8_t>(message.length() & 0xff) };

    if (message.size() > 0xffff)
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

    int r = write(fd_, sz, 2);
    if (r < 0)
        throw std::runtime_error("Error writing to serial.");
    r = write(fd_, message.data(), message.length());
    if (r < 0)
        throw std::runtime_error("Error writing to serial.");
}

template <typename T>
std::optional<T> Serial::receive_message() const
{
    uint8_t sz[2];
    ssize_t r = read(fd_, &sz, 2);
    if (r == 0)
        return {};
    else if (r == -1)
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

    T msg;
    msg.ParseFromArray(message, tsz);

    if (debugging_level_ != DebuggingLevel::NORMAL)
        printf("<- %s", msg.DebugString().c_str());

    return msg;
}

}