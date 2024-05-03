#include "serial.hh"

#include <cerrno>
#include <fcntl.h>
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

void Serial::configure_terminal_settings(uint32_t baud) const
{
    struct termios tio {};
    tcgetattr(fd_, &tio);
    cfmakeraw(&tio);
    cfsetspeed(&tio, baud);
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 0;
    if (tcsetattr(fd_, TCSANOW, &tio) != 0)
        throw std::runtime_error("Could not set terminal settings: "s + strerror(errno));

    fcntl(fd_, F_SETFL, FNDELAY);
}

}