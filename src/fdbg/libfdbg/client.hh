#ifndef CLIENT_HH_
#define CLIENT_HH_

#include <cstdint>
#include "serial.hh"

#include "fdbg/protobuf/to-computer.pb.h"
#include "fdbg/protobuf/to-debugger.pb.h"

namespace fdbg {

class DebuggerClient : public Serial {
public:
    explicit DebuggerClient(const char* port, uint32_t baudrate=115200);

    std::optional<ToDebugger> receive() const { return receive_message<ToDebugger>(); }

    void ack(uint32_t id) const;

private:
    void send(ToComputer const& msg) const { send_message(msg); }
};

}

#endif //CLIENT_HH_
