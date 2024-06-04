#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "libfdbg-server.h"

static uint8_t* ram;
static uint16_t pc = 0;
static uint8_t a = 0;
static bool z = false;
static uint8_t next_char = 0;

fdbg_ComputerStatus get_computer_status(FdbgServer* server)
{
    (void) server;

    fdbg_ComputerStatus cstatus = fdbg_ComputerStatus_init_zero;
    cstatus.pc = pc;
    cstatus.registers_count = 1;
    cstatus.registers[0] = a;
    cstatus.flags_count = 1;
    cstatus.flags[0] = z;
    return cstatus;
}

void reset(FdbgServer* server)
{
    (void) server;

    pc = 0;
}

void on_keypress(FdbgServer* server, const char* key)
{
    (void) server;

    next_char = key[0];
}

uint64_t step(FdbgServer* server, bool full, fdbg_Status* status)
{
    (void) server; (void) full;

    switch (ram[pc]) {
        case 0x1:  // IN
            a = next_char;
            next_char = 0;
            ++pc;
            break;
        case 0x2:  // BZ
            if (a == 0)
                pc = ram[pc+1];
            else
                pc += 2;
            break;
        case 0x3:  // OUT
            if (a != 0) {
                char text[] = { a, '\0' };
                fdbg_server_terminal_print(server, text);
                a = 0;
            }
            ++pc;
            break;
        case 0x4:  // JP
            pc = ram[pc+1];
            break;
        default:
            if (status)
                *status = fdbg_Status_CPU_INVALID_INSTRUCTION;
    }

    return pc;
}

bool next_instruction(FdbgServer* server, ADDR_TYPE* addr)
{
    (void) server;
    return false;
}

bool write_memory(FdbgServer* server, uint8_t nr, uint64_t pos, uint8_t* data, uint8_t sz, uint64_t* first_failed)
{
    (void) server;
    (void) first_failed;

    for (size_t i = 0; i < sz; ++i)
        ram[pos + i] = data[i];

    return true;
}

void read_memory(FdbgServer* server, uint8_t nr, uint64_t pos, uint8_t sz, uint8_t* out_data)
{
    (void) server;

    for (size_t i = 0; i < sz; ++i)
        out_data[i] = ram[pos + i];
}

int main()
{
    FdbgServer server;
    fdbg_server_init_pc(&server, 0x8f42, EMULATOR_BAUD_RATE);

    ram = calloc(1, 512);

    FdbgServerEvents events = {
            .get_computer_status = get_computer_status,
            .reset = reset,
            .step = step,
            .write_memory = write_memory,
            .read_memory = read_memory,
            .next_instruction = next_instruction,
            .on_keypress = on_keypress,
    };

    for (;;) {
        fdbg_server_next(&server, &events);
        fdbg_die_if_parent_dies();
    }
}
