#ifndef CONNECTION_HH_
#define CONNECTION_HH_

#include <cstdint>

#include "request.pb.h"
#include "response.pb.h"

enum class DebuggingLevel { NORMAL, DEBUG, TRACE };

class Connection {
public:
    Connection(const char* port, uint32_t baudrate);
    ~Connection();

    void     send_request(Request const& request) const;
    Response receive_response() const;

    void set_debugging_level(DebuggingLevel debugging_level) { debugging_level_ = debugging_level; }

private:
    int master_fd_ = -1;
    DebuggingLevel debugging_level_ = DebuggingLevel::NORMAL;
};

#endif