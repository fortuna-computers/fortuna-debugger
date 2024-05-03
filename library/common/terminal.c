#include "terminal.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#if __APPLE__
#  include <util.h>
#else
#  include <pty.h>
#  include <stdio.h>
#  include <termios.h>
#endif

int configure_terminal_settings(int fd, uint32_t baud)
{
    struct termios tio;
    tcgetattr(fd, &tio);
    cfmakeraw(&tio);
    cfsetspeed(&tio, baud);
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 0;
    if (tcsetattr(fd, TCSANOW, &tio) != 0)
        return -1;

    if (fcntl(fd, F_SETFL, FNDELAY) != 0)
        return -1;

    return 0;
}