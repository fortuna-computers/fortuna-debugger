#include "libfdbg-server.h"

#ifndef MICROCONTROLLER
#  if __APPLE__
#    include <util.h>
#  else
#    include <pty.h>
#  endif

#  include <errno.h>
#  include <stdio.h>
#  include <unistd.h>

#  include "../common/terminal.h"
#endif

#include <stdlib.h>

#include "to-computer.pb.h"
#include "to-debugger.pb.h"
#include "pb_decode.h"
#include "pb_encode.h"

typedef struct FdbgServer {
    uint16_t machine_id;
    FdbgServerIOCallbacks io_callbacks;
#ifndef MICROCONTROLLER
    int  fd;
    char port[256];
#endif
} FdbgServer;

FdbgServer* fdbg_server_init(uint16_t machine_id, FdbgServerIOCallbacks cb)
{
    FdbgServer* server = calloc(1, sizeof(FdbgServer));
    server->machine_id = machine_id;
    server->io_callbacks = cb;
    return server;
}

void fdbg_server_free(FdbgServer* server)
{
#ifndef MICROCONTROLLER
    close(server->fd);
#endif
    free(server);
}

static uint8_t fdbg_read_sync(FdbgServer* server)
{
    // TODO - timeout
    // TODO - sleep for a bit

    for (;;) {
        uint16_t c = server->io_callbacks.read_byte_async(server);
        if (c != SERIAL_NO_DATA)
            return c;
    }
}

static bool fdbg_receive_next_message(FdbgServer* server, fdbg_ToComputer* msg, bool* error)
{
    *error = false;

    uint16_t r = server->io_callbacks.read_byte_async(server);
    if (r == SERIAL_ERROR) {
        *error = true;
        return false;
    } if (r == SERIAL_NO_DATA) {
        return false;
    }

    uint8_t b0 = fdbg_read_sync(server);
    uint16_t sz = b0 | (r << 8);
    if (sz > MAX_MESSAGE_SZ) {
        *error = true;
        return false;
    }

    uint8_t buf[sz];
    for (size_t i = 0; i < sz; ++i)
        buf[i] = fdbg_read_sync(server);

    pb_istream_t stream = pb_istream_from_buffer(buf, sz);
    return pb_decode(&stream, fdbg_ToComputer_fields, msg);
}

static int fdbg_send_message(FdbgServer* server, fdbg_ToDebugger* msg)
{
    uint8_t buf[MAX_MESSAGE_SZ];

    pb_ostream_t stream = pb_ostream_from_buffer(buf, sizeof buf);
    bool status = pb_encode(&stream, fdbg_ToDebugger_fields, msg);
    if (status) {
        server->io_callbacks.write_byte(server, ((stream.bytes_written >> 8) & 0xff));
        server->io_callbacks.write_byte(server, stream.bytes_written & 0xff);

        for (size_t i = 0; i < stream.bytes_written; ++i)
            server->io_callbacks.write_byte(server, buf[i]);

        return 0;
    } else {
        return -1;
    }
}

int fdbg_server_next(FdbgServer* server, FdbgServerEvents* events)
{
    bool error = false;

    fdbg_ToComputer msg;
    if (fdbg_receive_next_message(server, &msg, &error)) {

        switch (msg.which_message) {
            case fdbg_ToComputer_ack_tag: {
                fdbg_ToDebugger rmsg = fdbg_ToDebugger_init_default;
                rmsg.which_message = fdbg_ToDebugger_ack_response_tag;
                rmsg.message.ack_response.id = server->machine_id;
                fdbg_send_message(server, &rmsg);
                break;
            }
        }

    }

    return error ? -1 : 0;
}

#ifndef MICROCONTROLLER

static uint16_t read_byte_async_pc(FdbgServer* server)
{
    uint8_t byte;
    ssize_t r = read(server->fd, &byte, 1);
    if (r == 0 || (r == -1 && errno == EAGAIN))
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

FdbgServer* fdbg_server_init_pc(uint16_t machine_id, uint32_t baud)
{
    int fd, slave_fd;

    char serial_port[1024];

    if (openpty(&fd, &slave_fd, serial_port, NULL, NULL) == -1)
        return NULL;

    if (configure_terminal_settings(fd, baud) < 0)
        return NULL;

    FdbgServer* server = fdbg_server_init(machine_id, (FdbgServerIOCallbacks) { read_byte_async_pc, write_byte_pc });
    server->fd = fd;
    snprintf(server->port, sizeof server->port, "%s", serial_port);
    return server;
}

const char* fdbg_server_serial_port(FdbgServer* server)
{
    return server->port;
}

#endif
