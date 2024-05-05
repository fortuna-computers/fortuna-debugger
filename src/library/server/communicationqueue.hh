#ifndef COMMUNICATION_QUEUE_HH_
#define COMMUNICATION_QUEUE_HH_

#include <cstdint>

#include <optional>
#include <string>

#include "library/protobuf/to-computer.pb.h"
#include "library/protobuf/to-debugger.pb.h"

class CommunicationQueue {
public:
    void connect(std::string const& port, uint32_t baudrate);

    void communicate();

    void send_message(fdbg::ToComputer const& msg);           // put message in outbox
    std::optional<fdbg::ToDebugger> receive_next_message();   // get message from inbox
    std::optional<fdbg::ToDebugger> receive_next_message_of_type(fdbg::ToDebugger::MessageCase messageType);
};

#endif