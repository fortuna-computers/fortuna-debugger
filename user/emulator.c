#include "emulator.h"

#include "machine.h"

static uint8_t ram[TOTAL_MAPPABLE_MEMORY];

void emulator_init()
{
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
