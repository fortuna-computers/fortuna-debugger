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
#include <string.h>

#include "pb_decode.h"
#include "pb_encode.h"

typedef struct FdbgServer {
    uint16_t              machine_id;
    FdbgServerIOCallbacks io_callbacks;
    ADDR_TYPE             breakpoints[MAX_BREAKPOINTS];
    bool                  running;
    ADDR_TYPE             next_bkp;
    uint32_t              run_steps;
    ADDR_TYPE             last_pc;
#ifndef MICROCONTROLLER
    int                   fd;
    char                  port[256];
#endif
} FdbgServer;

#define NO_BREAKPOINT ((ADDR_TYPE) -1)

FdbgServer* fdbg_server_init(uint16_t machine_id, FdbgServerIOCallbacks cb)
{
    FdbgServer* server = calloc(1, sizeof(FdbgServer));
    server->machine_id = machine_id;
    server->io_callbacks = cb;
    server->running = false;
    server->next_bkp = NO_BREAKPOINT;
    server->run_steps = 512;
    for (size_t i = 0; i < MAX_BREAKPOINTS; ++i)
        server->breakpoints[i] = NO_BREAKPOINT;
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

    uint16_t sz = server->io_callbacks.read_byte_async(server);
    if (sz == SERIAL_ERROR) {
        *error = true;
        return false;
    } else if (sz == SERIAL_NO_DATA) {
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
        server->io_callbacks.write_byte(server, stream.bytes_written & 0xff);

        for (size_t i = 0; i < stream.bytes_written; ++i)
            server->io_callbacks.write_byte(server, buf[i]);

        return 0;
    } else {
        return -1;
    }
}

static void fdbg_add_computer_status(FdbgServer* server, FdbgServerEvents* events, fdbg_ToDebugger* msg)
{
    fdbg_ComputerStatus cstatus = events->get_computer_status(server);

    msg->status = fdbg_Status_OK;
    msg->which_message = fdbg_ToDebugger_computer_status_tag;
    memcpy(&msg->message.computer_status, &cstatus, sizeof(fdbg_ComputerStatus));
}

static void fdbg_handle_msg_running(FdbgServer *server, FdbgServerEvents *events, fdbg_ToComputer *msg)
{
    bool error = false;
    fdbg_ToDebugger rmsg = fdbg_ToDebugger_init_default;
    rmsg.status = fdbg_Status_OK;

    switch ((*msg).which_message) {

        case fdbg_ToComputer_pause_tag: {
            server->running = false;
            server->next_bkp = NO_BREAKPOINT;
            fdbg_add_computer_status(server, events, &rmsg);
            break;
        }

        case fdbg_ToComputer_get_run_status_tag: {
            rmsg.which_message = fdbg_ToDebugger_run_status_tag;
            rmsg.message.run_status.running = true;
            rmsg.message.run_status.pc = server->last_pc;
            // TODO - add IO events
            break;
        }

        default: {
            rmsg.status = fdbg_Status_INVALID_MESSAGE;
            break;
        }
    }

    if (fdbg_send_message(server, &rmsg) != 0)
        error = true;

    if (error)
        rmsg.status = fdbg_Status_IO_SERIAL_ERROR;
}

static void fdbg_handle_msg_paused(FdbgServer *server, FdbgServerEvents *events, fdbg_ToComputer *msg)
{
    bool error = false;
    fdbg_ToDebugger rmsg = fdbg_ToDebugger_init_default;
    rmsg.status = fdbg_Status_OK;

    switch ((*msg).which_message) {

        case fdbg_ToComputer_ack_tag: {
            rmsg.which_message = fdbg_ToDebugger_ack_response_tag;
            rmsg.message.ack_response.id = server->machine_id;
            rmsg.message.ack_response.server_sz = sizeof(FdbgServer);
            rmsg.message.ack_response.to_computer_sz = sizeof(fdbg_ToComputer);
            rmsg.message.ack_response.to_debugger_sz = sizeof(fdbg_ToDebugger);
            break;
        }

        case fdbg_ToComputer_reset_tag: {
            events->reset(server);
            fdbg_add_computer_status(server, events, &rmsg);
            break;
        }

        case fdbg_ToComputer_step_tag: {
            events->step(server, (*msg).message.step.full);
            fdbg_add_computer_status(server, events, &rmsg);
            break;
        }

        case fdbg_ToComputer_run_tag: {
            if (msg->message.run.forever && events->run_forever)
                events->run_forever(server);   // should not return
            server->running = true;
            break;
        }

        case fdbg_ToComputer_next_tag: {
            server->next_bkp = events->next_instruction(server);
            server->running = true;
            break;
        }

        case fdbg_ToComputer_get_run_status_tag: {
            rmsg.which_message = fdbg_ToDebugger_run_status_tag;
            rmsg.message.run_status.running = false;
            rmsg.message.run_status.pc = events->get_computer_status(server).pc;
            break;
        }

        case fdbg_ToComputer_cycle_tag: {
            fdbg_CycleResponse response = events->cycle(server);
            rmsg.status = fdbg_Status_OK;
            rmsg.which_message = fdbg_ToDebugger_cycle_response_tag;
            memcpy(&rmsg.message.cycle_response, &response, sizeof(fdbg_CycleResponse));
            break;
        }

        case fdbg_ToComputer_write_memory_tag: {
            if ((*msg).message.write_memory.bytes.size > MAX_MEMORY_TRANSFER)
                (*msg).message.write_memory.bytes.size = MAX_MEMORY_TRANSFER;
            bool status = false;
            ADDR_TYPE first_failed = (*msg).message.write_memory.initial_addr;
            if (events->write_memory) {
                status = events->write_memory(server,
                                              (*msg).message.write_memory.initial_addr, (*msg).message.write_memory.bytes.bytes,
                                              (*msg).message.write_memory.bytes.size, &first_failed);
            }

            rmsg.which_message = fdbg_ToDebugger_write_memory_response_tag;
            rmsg.status = status ? fdbg_Status_OK : fdbg_Status_ERR_WRITING_MEMORY;
            if (!status)
                rmsg.message.write_memory_response.first_failed_pos = first_failed;
            break;
        }

        case fdbg_ToComputer_read_memory_tag: {
            rmsg.status = fdbg_Status_OK;
            if ((*msg).message.read_memory.sz > MAX_MEMORY_TRANSFER)
                (*msg).message.read_memory.sz = MAX_MEMORY_TRANSFER;
            uint8_t buf[(*msg).message.read_memory.sz];
            if (events->read_memory)
                events->read_memory(server, (*msg).message.read_memory.initial_addr, (*msg).message.read_memory.sz, buf);
            else
                memset(buf, 0, (*msg).message.read_memory.sz);

            rmsg.which_message = fdbg_ToDebugger_read_memory_response_tag;
            rmsg.message.read_memory_response.initial_pos = (*msg).message.read_memory.initial_addr;
            rmsg.message.read_memory_response.bytes.size = (*msg).message.read_memory.sz;
            memcpy(rmsg.message.read_memory_response.bytes.bytes, buf, (*msg).message.read_memory.sz);
            break;
        }

        case fdbg_ToComputer_breakpoint_tag: {
            rmsg.status = fdbg_Status_OK;
            switch ((*msg).message.breakpoint.action) {
                case fdbg_Breakpoint_Action_ADD:
                    for (size_t i = 0; i < MAX_BREAKPOINTS; ++i) {
                        if (server->breakpoints[i] == 0)
                            continue;  // no breakpoints on zero
                        if (server->breakpoints[i] == (ADDR_TYPE) (*msg).message.breakpoint.address)
                            goto bkp_done;
                        if (server->breakpoints[i] == NO_BREAKPOINT) {
                            server->breakpoints[i] = (ADDR_TYPE) (*msg).message.breakpoint.address;
                            goto bkp_done;
                        }
                    }
                    rmsg.status = fdbg_Status_TOO_MANY_BREAKPOINTS;
                    break;
                case fdbg_Breakpoint_Action_REMOVE:
                    for (size_t i = 0; i < MAX_BREAKPOINTS; ++i) {
                        if (server->breakpoints[i] == (ADDR_TYPE) (*msg).message.breakpoint.address) {
                            server->breakpoints[i] = NO_BREAKPOINT;
                            goto bkp_done;
                        }
                    }
                    break;
                case fdbg_Breakpoint_Action_CLEAR_ALL:
                    for (size_t i = 0; i < MAX_BREAKPOINTS; ++i)
                        server->breakpoints[i] = NO_BREAKPOINT;
                    break;
            }
bkp_done:
            rmsg.which_message = fdbg_ToDebugger_breakpoint_list_tag;
            size_t j = 0;
            for (size_t i = 0; i < MAX_BREAKPOINTS; ++i) {
                if (server->breakpoints[i] != NO_BREAKPOINT) {
                    ++rmsg.message.breakpoint_list.addr_count;
                    rmsg.message.breakpoint_list.addr[j++] = server->breakpoints[i];
                }
            }

            break;
        }

        default: {
            rmsg.status = fdbg_Status_INVALID_MESSAGE;
            break;
        }

    }

    if (fdbg_send_message(server, &rmsg) != 0)
        error = true;

    if (error)
        rmsg.status = fdbg_Status_IO_SERIAL_ERROR;
}

static void fdbg_run_steps(FdbgServer* server, FdbgServerEvents* events)
{
    for (size_t i = 0; i < server->run_steps; ++i) {

        server->last_pc = events->step(server, false);

        if (server->last_pc == server->next_bkp) {
            server->next_bkp = NO_BREAKPOINT;
            server->running = false;
            return;
        }

        for (size_t j = 0; j < MAX_BREAKPOINTS; ++j) {
            if (server->last_pc == server->breakpoints[j]) {
                server->running = false;
                return;
            }
        }
    }
}

void fdbg_server_next(FdbgServer* server, FdbgServerEvents* events)
{
    if (server->running)
        fdbg_run_steps(server, events);

    bool error = false;

    fdbg_ToComputer msg;
    if (fdbg_receive_next_message(server, &msg, &error)) {

        if (server->running)
            fdbg_handle_msg_running(server, events, &msg);
        else
            fdbg_handle_msg_paused(server, events, &msg);
    }
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

    char serial_port[256];

    if (openpty(&fd, &slave_fd, serial_port, NULL, NULL) == -1)
        return NULL;

    if (configure_terminal_settings(fd, baud) < 0)
        return NULL;

    FdbgServer* server = fdbg_server_init(machine_id, (FdbgServerIOCallbacks) { read_byte_async_pc, write_byte_pc });
    server->fd = fd;
    snprintf(server->port, sizeof server->port, "%s", serial_port);

    char filename[512];
    snprintf(filename, sizeof filename, "/tmp/fdbg.%d", getpid());
    FILE* f = fopen(filename, "w");
    fprintf(f, "%s", server->port);
    fclose(f);

    return server;
}

const char* fdbg_server_serial_port(FdbgServer* server)
{
    return server->port;
}

void fdbg_die_if_parent_dies()
{
    if (getppid() == 1) {
        char filename[512];
        snprintf(filename, sizeof filename, "/tmp/fdbg.%d", getpid());
        unlink(filename);
        exit(0);
    }
}

#endif
