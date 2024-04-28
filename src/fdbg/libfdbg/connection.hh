#ifndef CONNECTION_HH_
#define CONNECTION_HH_

#include <cstdint>

#include "request.pb.h"
#include "response.pb.h"

class Connection {
public:
    Connection(const char* port, uint32_t baudrate);
    ~Connection();

    void     send_request(Request const& request) const;
    Response receive_response() const;

private:
    int master_fd;
};

#endif