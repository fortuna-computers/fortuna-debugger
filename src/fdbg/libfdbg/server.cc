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

void ComputerServer::send_write_memory_confirmation(bool ok, uint64_t first_failed_pos) const
{
    ToDebugger msg;

    auto write_memory_conf = new WriteMemoryConfirmation();
    write_memory_conf->set_error(!ok);
    if (!ok)
        write_memory_conf->set_first_failed_pos(first_failed_pos);
    msg.set_allocated_write_memory_confirmation(write_memory_conf);

    send(msg);
}

void ComputerServer::send_memory_update(uint64_t pos, std::vector<uint8_t> const& bytes) const
{
    ToDebugger msg;

    auto memory_update = new MemoryUpdate();
    memory_update->set_initial_pos(pos);
    memory_update->set_bytes(bytes.data(), bytes.size());
    msg.set_allocated_memory_update(memory_update);

    send(msg);
}

}