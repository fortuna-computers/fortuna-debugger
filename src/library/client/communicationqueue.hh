#ifndef COMMUNICATION_QUEUE_HH_
#define COMMUNICATION_QUEUE_HH_

#include <cstdint>

#include <list>
#include <queue>
#include <optional>
#include <string>

#include "library/protobuf/to-computer.pb.h"
#include "library/protobuf/to-debugger.pb.h"
#include "debuglevel.hh"

class CommunicationQueue {
public:
    ~CommunicationQueue();

    void connect(std::string const& port, uint32_t baudrate);

    void send_message(fdbg::ToComputer const& msg);           // put message in outbox
    std::optional<fdbg::ToDebugger> receive_next_message();   // get message from inbox
    std::optional<fdbg::ToDebugger> receive_next_message_of_type(fdbg::ToDebugger::MessageCase messageType);

    void set_debugging_level(DebuggingLevel debugging_level) { debugging_level_ = debugging_level; }

private:
    void communicate();
    void receive_messages();
    void send_messages();

    bool                         server_ready_ = false;
    std::list<fdbg::ToDebugger>  inbox_;
    std::queue<fdbg::ToComputer> outbox_;
    int                          fd_;
    DebuggingLevel               debugging_level_ = DebuggingLevel::NORMAL;
};

#endif