#define RANDOM_MEMORY_AT_START

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "libfdbg-server.h"

static uint8_t* ram;
static uint16_t pc = 0;
static bool     has_values = false;
static uint16_t a = 0;
static bool eq = true, z = false;

fdbg_ComputerStatus get_computer_status(FdbgServer* server)
{
    (void) server;

    fdbg_ComputerStatus cstatus = fdbg_ComputerStatus_init_zero;
    cstatus.pc = pc;
    if (has_values) {
        cstatus.registers_count = 1;
        cstatus.registers[0] = a;
        cstatus.flags_count = 2;
        cstatus.flags[0] = eq;
        cstatus.flags[1] = z;
        cstatus.stack.size = 8;
        memcpy(cstatus.stack.bytes, ram, 8);
    }
    return cstatus;
}

void reset(FdbgServer* server)
{
    (void) server;

    pc = 0;
}

fdbg_CycleResponse cycle(FdbgServer* server)
{
    (void) server;

    fdbg_CycleResponse c = fdbg_CycleResponse_init_zero;
    c.bytes_count = 2;
    c.bytes[0].has = true;
    c.bytes[0].value = 0xa4;
    c.bytes[1].has = false;
    c.bits_count = 2;
    c.bits[0] = true;
    c.bits[1] = false;
    return c;
}

uint64_t next_instruction(FdbgServer* server)
{
    (void) server;
    return pc + 2;
}

uint64_t step(FdbgServer* server, bool full)
{
    (void) server; (void) full;

    // if (pc == 1)
        fdbg_server_push_event(server, 0x4812, 0xaf);

    ++pc;
    has_values = full;

    return pc;
}

bool write_memory(FdbgServer* server, uint64_t pos, uint8_t* data, uint8_t sz, uint64_t* first_failed)
{
    (void) server;
    (void) first_failed;

    for (size_t i = 0; i < sz; ++i)
        ram[pos + i] = data[i];

    return true;
}

void read_memory(FdbgServer* server, uint64_t pos, uint8_t sz, uint8_t* out_data)
{
    (void) server;

    for (size_t i = 0; i < sz; ++i)
        out_data[i] = ram[pos + i];
}

int main()
{
    FdbgServer* server = fdbg_server_init_pc(0x38f7, EMULATOR_BAUD_RATE);

    ram = calloc(4, 1024);

#ifdef RANDOM_MEMORY_AT_START
    srand(time(0));
    for (size_t i = 0; i < 4 * 1024; ++i)
        ram[i] = rand() & 0xff;
#endif

    FdbgServerEvents events = {
            .get_computer_status = get_computer_status,
            .reset = reset,
            .step = step,
            .cycle = cycle,
            .write_memory = write_memory,
            .read_memory = read_memory,
            .next_instruction = next_instruction,
    };

    for (;;) {
        fdbg_server_next(server, &events);
        fdbg_die_if_parent_dies();
    }
}
