#include "server.hh"

#if __APPLE__
#  include <util.h>
#else
#  include <pty.h>
#endif

#include <stdexcept>
#include <string>
using namespace std::string_literals;

namespace fdbg {

ComputerServer::ComputerServer(uint32_t baudrate)
{
    int slave_fd;

    char serial_port[1024];

    if (openpty(&fd_, &slave_fd, serial_port, nullptr, nullptr) == -1)
        throw std::runtime_error("Could not open serial port "s + serial_port);

    port_ = serial_port;

    configure_terminal_settings(baudrate);
}

void ComputerServer::send_ack_response(uint32_t id) const
{
    ToDebugger msg;

    auto ack_response = new AckResponse();
    ack_response->set_id(id);
    msg.set_allocated_ack_response(ack_response);

    send(msg);
}

}