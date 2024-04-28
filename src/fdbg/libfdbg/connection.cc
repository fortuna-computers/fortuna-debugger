#include "connection.hh"

#include <unistd.h>

#if __APPLE__
#  include <util.h>
#else
#  include <pty.h>
#  include <stdio.h>
#  include <termios.h>
#endif

#include <stdexcept>
#include <string>
using namespace std::string_literals;

Connection::Connection(char const *port, uint32_t baudrate)
{
    int slave_fd;

    char* serial_port = strdup(port);
    if (openpty(&master_fd, &slave_fd, serial_port, nullptr, nullptr) == -1)
        throw std::runtime_error("Could not open serial port "s + serial_port);

    // configure terminal settings
    struct termios tio {};
    tcgetattr(master_fd, &tio);
    cfmakeraw(&tio);
    tcsetattr(master_fd, TCSANOW, &tio);
}

Connection::~Connection()
{

}

void Connection::send_request(Request const &request) const
{

}

Response Connection::receive_response() const
{
    return Response();
}
