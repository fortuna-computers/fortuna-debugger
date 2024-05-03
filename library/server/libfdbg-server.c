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
    FdbgServerIOCallbacks io_callbacks;
#ifndef MICROCONTROLLER
    int  fd;
    char port[256];
#endif
} FdbgServer;

#include "to-debugger.pb.h"

FdbgServer* fdbg_server_init(FdbgServerIOCallbacks cb)
{
    FdbgServer* server = calloc(1, sizeof(FdbgServer));
    server->io_callbacks = cb;
    return server;
}

void fdbg_server_free(FdbgServer* server)
{
    close(server->fd);
    free(server);
}

#ifndef MICROCONTROLLER

static uint16_t read_byte_async_pc(FdbgServer* server)
{
    uint8_t byte;
    ssize_t r = read(server->fd, &byte, 1);
    if (r == 0)
        return SERIAL_NO_DATA;
    else if (r < 0)
        return SERIAL_ERROR;
    else
        return byte;
}

static void write_byte_pc(FdbgServer* server, uint8_t data)
{
    write(server->fd, &data, 1);
}

FdbgServer* fdbg_server_init_pc(uint32_t baud)
{
    int fd, slave_fd;

    char serial_port[1024];

    if (openpty(&fd, &slave_fd, serial_port, NULL, NULL) == -1)
        return NULL;

    if (configure_terminal_settings(fd, baud) < 0)
        return NULL;

    FdbgServer* server = fdbg_server_init((FdbgServerIOCallbacks) { read_byte_async_pc, write_byte_pc });
    server->fd = fd;
    snprintf(serial_port, sizeof server->port, "%s", serial_port);
    return server;
}

const char* fdbg_server_serial_port(FdbgServer* server)
{
    return server->port;
}

#endif
