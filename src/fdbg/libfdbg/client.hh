#ifndef CLIENT_HH_
#define CLIENT_HH_

#include <cstdint>

#include <functional>

#include "serial.hh"

#include "fdbg/protobuf/to-computer.pb.h"
#include "fdbg/protobuf/to-debugger.pb.h"

namespace fdbg {

class DebuggerClient : public Serial {
public:
    static const int EMULATOR_BAUD = 921600;

    void        connect(std::string const& port, uint32_t baudrate=115200);

    std::optional<ToDebugger> receive() const { return receive_message<ToDebugger>(); }

    void ack_sync(uint32_t id) const;
    void write_memory(uint64_t pos, std::vector<uint8_t> const& area) const;
    void write_memory_sync(uint64_t pos, std::vector<uint8_t> const& area) const;
    void read_memory_request(uint64_t pos, uint16_t sz) const;

private:
    void send(ToComputer const& msg) const { send_message(msg); }
    std::string read_port_from_emulator() const;

    ToDebugger wait_for_response(std::function<bool(ToDebugger const& msg)> check_function) const;
};

}

#endif //CLIENT_HH_
