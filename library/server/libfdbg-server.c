#include "libfdbg-server.h"

#if __APPLE__
#  include <util.h>
#else
#  include <pty.h>
#endif

#include <stdlib.h>
#include <unistd.h>

#include "../common/terminal.h"

typedef struct FdbgServer {
    int            fd;
    char           port[256];
    DebuggingLevel debugging_level;
} FdbgServer;

#include "to-debugger.pb.h"

FdbgServer* fdbg_server_init(uint32_t baud)
{
    int fd, slave_fd;

    char serial_port[1024];

    if (openpty(&fd, &slave_fd, serial_port, NULL, NULL) == -1)
        return NULL;

    if (configure_terminal_settings(fd, baud) < 0)
        return NULL;

    FdbgServer* server = calloc(1, sizeof(FdbgServer));
    server->fd = fd;
    strncpy(server->port, serial_port, sizeof server->port);
    server->debugging_level = DL_NORMAL;
    return server;
}

void fdbg_server_free(FdbgServer* server)
{
    close(server->fd);
    free(server);
}

const char* fdbg_server_serial_port(FdbgServer* server)
{
    return server->port;
}

void fdbg_server_set_debugging_level(FdbgServer* server, DebuggingLevel d)
{
    server->debugging_level = d;
}