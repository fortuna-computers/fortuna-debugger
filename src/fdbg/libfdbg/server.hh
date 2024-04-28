#ifndef SERVER_HH_
#define SERVER_HH_

#include <cstdint>

class Server {
public:
    Server(const char* port, uint32_t baudrate);
    ~Server();


};

#endif