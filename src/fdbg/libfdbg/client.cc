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

DebuggerClient::DebuggerClient(char const *port, uint32_t baudrate)
{
    fd_ = open(port, O_RDWR);
    if (fd_ < 0)
        throw std::runtime_error("Could not open serial port "s + port);

    configure_terminal_settings(baudrate);
}

void DebuggerClient::ack(uint32_t id) const
{
    ToComputer msg;

    msg.set_allocated_ack(new Ack());

    send(msg);

    std::optional<ToDebugger> response;
    auto initial_time = hrc::now();
    for (;;) {
        response = receive();
        if (!response)
            std::this_thread::sleep_for(1ms);
        else if (response.value().has_ack_response())
            break;

        if (hrc::now() > initial_time + 2s)
            throw Timeout();
    }

    if (response->ack_response().id() != id)
        throw InvalidId();
}

}
