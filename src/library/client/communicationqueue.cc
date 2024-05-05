#include "communicationqueue.hh"

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

#include <string>
#include <thread>

extern "C" {
#include "../common/common.h"
#include "../common/terminal.h"
}

using namespace std::chrono_literals;
using namespace std::string_literals;
using hrc = std::chrono::high_resolution_clock;

using namespace std::string_literals;

void CommunicationQueue::connect(std::string const& port, uint32_t baudrate)
{
    fd_ = open(port.c_str(), O_RDWR);
    if (fd_ < 0)
        throw std::runtime_error("Could not open serial port "s + port + ": " + strerror(errno));

    configure_terminal_settings(fd_, baudrate);

    server_ready_ = true;
}

CommunicationQueue::~CommunicationQueue()
{
    if (fd_ != -1)
        close(fd_);
}

void CommunicationQueue::communicate()
{
    receive_messages();

    if (server_ready_)
        send_messages();
}

void CommunicationQueue::receive_messages()
{
    for (;;) {
        uint8_t sz;
        ssize_t r = read(fd_, &sz, 1);
        if (r == 0)
            break;  // no more messages
        else if (r == -1 && errno != EAGAIN)
            throw std::runtime_error("Error reading from serial: "s + strerror(errno));

        if (debugging_level_ == DebuggingLevel::TRACE)
            printf("<= [%02hhX]", sz);

        uint8_t message[sz];

        auto start = hrc::now();
        int count = 0;
        do {
            if (hrc::now() > start + 3s)
                throw std::runtime_error("Waiting for more than 3 seconds for a whole message - only received part of it.");
            count += read(fd_, &message[count], sz);
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

        if (msg.message_case() == fdbg::ToDebugger::kReady)
            server_ready_ = true;
        else
            inbox_.emplace_back(std::move(msg));
    }
}

void CommunicationQueue::send_messages()
{
    while (!outbox_.empty()) {
        server_ready_ = false;

        auto const& msg = outbox_.front();
        outbox_.pop();

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

}

void CommunicationQueue::send_message(fdbg::ToComputer const& msg)
{
    outbox_.push(msg);
    communicate();
}

std::optional<fdbg::ToDebugger> CommunicationQueue::receive_next_message()
{
    communicate();

    if (inbox_.empty())
        return {};
    auto msg = std::optional<fdbg::ToDebugger>{ inbox_.front() };
    inbox_.pop_front();
    return msg;
}

std::optional<fdbg::ToDebugger> CommunicationQueue::receive_next_message_of_type(fdbg::ToDebugger::MessageCase messageType)
{
    communicate();

    for (auto it = inbox_.begin(); it != inbox_.end(); ++it) {
        if (it->message_case() == messageType) {
            auto msg = std::optional<fdbg::ToDebugger> { *it };
            inbox_.erase(it);
            return msg;
        }
    }
    return {};
}
