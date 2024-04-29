#ifndef SERVER_HH_
#define SERVER_HH_

#include <cstdint>
#include "serial.hh"

#include "fdbg/protobuf/to-debugger.pb.h"
#include "fdbg/protobuf/to-computer.pb.h"

namespace fdbg {

class ComputerServer : public Serial {
public:
    explicit ComputerServer(uint32_t baudrate=115200);

    std::string const &port() const { return port_; }

    std::optional<ToComputer> receive() const { return receive_message<ToComputer>(); }

    void send_ack_response(uint32_t id) const;

private:
    void send(ToDebugger const& msg) const { send_message(msg); }

    std::string port_;
};

}

#endif //SERVER_HH_
