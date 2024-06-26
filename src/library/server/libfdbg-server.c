#include "libfdbg-server.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

#include "srvcomm.h"

#ifndef MICROCONTROLLER
#  if __APPLE__
#    include <util.h>
#  else
#    include <pty.h>
#  endif

#endif

#include "pb_decode.h"
#include "pb_encode.h"

#define MIN(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

#define NO_BREAKPOINT ((ADDR_TYPE) -1)

#define TIME_TO_RECEIVE_MESSAGE 2000  // ms

void fdbg_server_init(FdbgServer* server, uint16_t machine_id, FdbgServerIOCallbacks cb)
{
    memset(server, 0, sizeof(FdbgServer));
    server->machine_id = machine_id;
    server->io_callbacks = cb;
    server->running = false;
    server->next_bkp = NO_BREAKPOINT;
    server->run_steps = 512;
    server->run_status = fdbg_Status_OK;
    for (size_t i = 0; i < MAX_BREAKPOINTS; ++i)
        server->breakpoints[i] = NO_BREAKPOINT;
}

void fdbg_server_close(FdbgServer* server)
{
#ifndef MICROCONTROLLER
    close(server->fd);
#endif
}

static void fdbg_add_computer_status(FdbgServer* server, FdbgServerEvents* events, fdbg_ToDebugger* msg)
{
    fdbg_ComputerStatus cstatus = events->get_computer_status(server);

    msg->which_message = fdbg_ToDebugger_computer_status_tag;
    memcpy(&msg->message.computer_status, &cstatus, sizeof(fdbg_ComputerStatus));
    msg->message.computer_status.events_count = MIN(server->event_count, MAX_EVENTS_STEP);
    memcpy(msg->message.computer_status.events, server->event_queue, MIN(server->event_count, MAX_EVENTS_STEP) * sizeof(fdbg_ComputerEvent));
    server->event_count = 0;
}

static void fdbg_execute_user_events(FdbgServer *server, FdbgServerEvents *events, size_t count, fdbg_UserEvent *user_events)
{
    for (size_t i = 0; i < count; ++i) {
        switch (user_events[i].which_type) {
            case fdbg_UserEvent_TerminalKeypress_text_tag:
                if (events->on_keypress)
                    events->on_keypress(server, user_events[i].type.terminal_keypress.text);
                break;
        }
    }
}

static void fdbg_build_run_status_message(FdbgServer* server, fdbg_ToDebugger* rmsg)
{
    rmsg->status = server->run_status;
    rmsg->which_message = fdbg_ToDebugger_run_status_tag;
    rmsg->message.run_status.running = server->running;
    rmsg->message.run_status.pc = server->last_pc;

    rmsg->message.run_status.events_count = MIN(server->event_count, MAX_EVENTS_STATUS);
    memcpy(rmsg->message.run_status.events, server->event_queue, MIN(server->event_count, MAX_EVENTS_STATUS) * sizeof(fdbg_ComputerEvent));
    server->event_count = 0;

    server->run_status = fdbg_Status_OK;
}

static void fdbg_handle_msg_running(FdbgServer *server, FdbgServerEvents *events, fdbg_ToComputer *msg)
{
    bool error = false;
    fdbg_ToDebugger rmsg = fdbg_ToDebugger_init_default;
    rmsg.status = fdbg_Status_OK;

    switch (msg->which_message) {

        case fdbg_ToComputer_pause_tag: {
            server->running = false;
            server->next_bkp = NO_BREAKPOINT;
            fdbg_add_computer_status(server, events, &rmsg);
            break;
        }

        case fdbg_ToComputer_get_run_status_tag: {
            fdbg_execute_user_events(server, events, msg->message.get_run_status.user_events_count, msg->message.get_run_status.user_events);
            fdbg_build_run_status_message(server, &rmsg);
            break;
        }

        default: {
            rmsg.status = fdbg_Status_INVALID_MESSAGE;
            break;
        }
    }

    if (comm_send_message(server, &rmsg) != 0)
        error = true;

    if (error)
        rmsg.status = fdbg_Status_IO_SERIAL_ERROR;
}

static void fdbg_handle_msg_paused(FdbgServer *server, FdbgServerEvents *events, fdbg_ToComputer *msg)
{
    bool error = false;
    fdbg_ToDebugger rmsg = fdbg_ToDebugger_init_default;
    rmsg.status = fdbg_Status_OK;

    switch (msg->which_message) {

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
            server->event_count = 0;
            fdbg_add_computer_status(server, events, &rmsg);
            break;
        }

        case fdbg_ToComputer_step_tag: {
            fdbg_execute_user_events(server, events, msg->message.step.user_events_count, msg->message.step.user_events);
            events->step(server, msg->message.step.full, &rmsg.status);
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
            ADDR_TYPE addr;
            bool is_subroutine = events->next_instruction(server, &addr);
            if (is_subroutine) {
                server->next_bkp = addr;
                server->running = true;
            } else {
                server->last_pc = events->step(server, false, &rmsg.status);
            }
            fdbg_build_run_status_message(server, &rmsg);
            break;
        }

        case fdbg_ToComputer_get_run_status_tag: {
            fdbg_execute_user_events(server, events, msg->message.get_run_status.user_events_count, msg->message.get_run_status.user_events);
            fdbg_build_run_status_message(server, &rmsg);
            break;
        }

        case fdbg_ToComputer_cycle_tag: {
            if (events->cycle) {
                fdbg_CycleResponse response = events->cycle(server);
                rmsg.status = fdbg_Status_OK;
                memcpy(&rmsg.message.cycle_response, &response, sizeof(fdbg_CycleResponse));
            } else {
                rmsg.status = fdbg_Status_METHOD_NOT_IMPLEMENTED;
            }
            rmsg.which_message = fdbg_ToDebugger_cycle_response_tag;
            break;
        }

        case fdbg_ToComputer_write_memory_tag: {
            if ((*msg).message.write_memory.bytes.size > MAX_MEMORY_TRANSFER)
                (*msg).message.write_memory.bytes.size = MAX_MEMORY_TRANSFER;
            bool status = false;
            ADDR_TYPE first_failed = (*msg).message.write_memory.initial_addr;
            if (events->write_memory) {
                status = events->write_memory(server,
                                              (*msg).message.write_memory.memory_nr,
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
                events->read_memory(server, (*msg).message.read_memory.memory_nr,
                                    (*msg).message.read_memory.initial_addr, (*msg).message.read_memory.sz, buf);
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

        case fdbg_ToComputer_interrupt_tag:
            if (events->interrupt)
                events->interrupt(server, msg->message.interrupt.number);
            else
                rmsg.status = fdbg_Status_METHOD_NOT_IMPLEMENTED;
            break;

        default: {
            rmsg.status = fdbg_Status_INVALID_MESSAGE;
            break;
        }

    }

    if (comm_send_message(server, &rmsg) != 0)
        error = true;

    if (error)
        rmsg.status = fdbg_Status_IO_SERIAL_ERROR;
}

static void fdbg_run_steps(FdbgServer* server, FdbgServerEvents* events)
{
    fdbg_Status status = fdbg_Status_OK;

    for (size_t i = 0; i < server->run_steps; ++i) {

        if (server->event_count == (MAX_EVENTS_STATUS - 1))
            return;  // we're not running any steps while there are pending events

        server->last_pc = events->step(server, false, &status);

        if (status != fdbg_Status_OK) {
            server->running = false;
            server->run_status = status;
            return;
        }

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
    if (comm_receive_next_message(server, &msg, &error)) {

        if (server->running)
            fdbg_handle_msg_running(server, events, &msg);
        else
            fdbg_handle_msg_paused(server, events, &msg);
    }
}

bool fdbg_server_push_event(FdbgServer* server, fdbg_ComputerEvent* event)
{
    if (server->event_count < MAX_EVENTS_STATUS - 1) {
        server->event_queue[server->event_count++] = *event;
        return true;
    } else {
        return false;
    }
}

bool fdbg_server_terminal_print(FdbgServer* server, const char* text)
{
    fdbg_ComputerEvent event = fdbg_ComputerEvent_init_zero;
    event.which_type = fdbg_ComputerEvent_terminal_print_tag;
    strncpy(event.type.terminal_print.text, text, 8);
    return fdbg_server_push_event(server, &event);
}

void fdbg_debug(FdbgServer* server, const char* fmt, ...)
{
    fdbg_ToDebugger rmsg = fdbg_ToDebugger_init_default;
    rmsg.status = fdbg_Status_OK;
    rmsg.which_message = fdbg_ToDebugger_debug_tag;

    va_list args;
    va_start(args, fmt);
    vsnprintf(rmsg.message.debug.text, DEBUG_SZ, fmt, args);
    va_end(args);

    comm_send_message(server, &rmsg);
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

static int configure_terminal_settings(int fd, uint32_t baud)
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

bool fdbg_server_init_pc(FdbgServer* server, uint16_t machine_id, uint32_t baud)
{
    int fd, slave_fd;

    char serial_port[256];

    if (openpty(&fd, &slave_fd, serial_port, NULL, NULL) == -1)
        return false;

    if (configure_terminal_settings(fd, baud) < 0)
        return false;

    fdbg_server_init(server, machine_id, (FdbgServerIOCallbacks) { read_byte_async_pc, write_byte_pc });
    server->fd = fd;
    snprintf(server->port, sizeof server->port, "%s", serial_port);

    char filename[512];
    snprintf(filename, sizeof filename, "/tmp/fdbg.%d", getpid());
    FILE* f = fopen(filename, "w");
    fprintf(f, "%s", server->port);
    fclose(f);

    return true;
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