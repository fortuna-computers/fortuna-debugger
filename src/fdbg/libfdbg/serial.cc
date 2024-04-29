#include "serial.hh"

#include <cerrno>
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

namespace fdbg {

Serial::~Serial()
{
    close(fd_);
}

void Serial::configure_terminal_settings(uint32_t baud)
{
    struct termios tio {};
    tcgetattr(fd_, &tio);
    cfmakeraw(&tio);
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 0;
    cfsetspeed(&tio, baud);
    if (tcsetattr(fd_, TCSANOW, &tio) != 0)
        throw std::runtime_error("Could not set terminal settings: "s + strerror(errno));
}

}