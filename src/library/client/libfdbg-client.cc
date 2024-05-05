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

std::optional<fdbg::ToDebugger> FdbgClient::receive_message()
{
    return comm_queue_.receive_next_message();
}

fdbg::ToDebugger FdbgClient::wait_for_message_of_type(fdbg::ToDebugger::MessageCase messageType)
{
    auto start = hrc::now();
    for (;;) {
        auto response = comm_queue_.receive_next_message_of_type(fdbg::ToDebugger::kAckResponse);
        if (response)
            return response.value();
        if (hrc::now() > start + 3s)
            throw std::runtime_error("Response not received in 3 seconds");
    }

}

void FdbgClient::ack_sync(uint32_t id)
{
    fdbg::ToComputer msg;
    msg.set_allocated_ack(new fdbg::Ack());
    comm_queue_.send_message(msg);

    auto response = wait_for_message_of_type(fdbg::ToDebugger::kAckResponse);
    if (response.ack_response().id() != id)
        throw std::runtime_error("Invalid id received from server during ack");
}

void FdbgClient::write_memory_sync(uint64_t pos, std::vector<uint8_t> const& data, bool validate)
{
    /*
    if (data.size() > MAX_MEMORY_TRANSFER)
        throw std::runtime_error("Cannot write more than " + std::to_string(MAX_MEMORY_TRANSFER) + " bytes at time.");

    auto write_memory = new fdbg::WriteMemory();
    write_memory->set_initial_addr(pos);
    write_memory->set_bytes(data.data(), data.size());
    write_memory->set_validate(validate);

    fdbg::ToComputer msg;
    msg.set_allocated_write_memory(write_memory);
    send_message(msg);

    auto response = wait_for_response([](fdbg::ToDebugger const& msg) { return msg.has_write_memory_confirmation(); });
    if (response.write_memory_confirmation().error())
        throw std::runtime_error(std::format("Error writing memory: first byte failed is 0x{:x}", response.write_memory_confirmation().first_failed_pos()));
        */
}

void FdbgClient::read_memory_async(uint64_t pos, uint8_t sz)
{
    /*
    if (sz > MAX_MEMORY_TRANSFER)
        throw std::runtime_error("Cannot read more than " + std::to_string(MAX_MEMORY_TRANSFER) + " bytes at time.");

    auto read_memory = new fdbg::ReadMemory();
    read_memory->set_initial_addr(pos);
    read_memory->set_sz(sz);

    fdbg::ToComputer msg;
    msg.set_allocated_read_memory(read_memory);
    send_message(msg);
     */
}

std::vector<uint8_t> FdbgClient::read_memory_sync(uint64_t pos, uint8_t sz)
{
    /*
    read_memory_async(pos, sz);

    auto response = wait_for_response([&pos](fdbg::ToDebugger const& msg) {
        return msg.has_memory_update() && msg.memory_update().initial_pos() == pos;
    });
    return { response.memory_update().bytes().begin(), response.memory_update().bytes().end() };
     */
}
