#include "emulator.h"

#include <time.h>
#include <stdlib.h>

#include "machine.h"

static uint8_t* ram;

void emulator_init()
{
    ram = malloc(total_mappable_memory());

    srand(time(0));
    for (size_t i = 0; i < total_mappable_memory(); ++i)
        ram[i] = rand() & 0xff;
}

void emulator_reset()
{
}

void emulator_ram_set(uint64_t pos, uint8_t data)
{
    ram[pos] = data;
}

uint8_t emulator_ram_get(uint32_t pos)
{
    return ram[pos];
}

