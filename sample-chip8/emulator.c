#define RANDOM_MEMORY_AT_START

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "libfdbg-server.h"

static uint8_t* ram;

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
            .write_memory = write_memory,
            .read_memory = read_memory,
    };

    for (;;) {
        if (fdbg_server_next(server, &events) != 0) {
            fprintf(stderr, "server: error reading data\n");
            exit(1);
        }

        fdbg_die_if_parent_dies();
    }
}
