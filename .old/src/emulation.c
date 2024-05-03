#include "emulation.h"

uint8_t emulator_memory_get(EmulatorCB* e, uint64_t pos)
{
    // Read a memory byte from a *mapped* memory address.
#error Implement this function
    if (pos < e->machine_characteristics()->total_actual_memory) {
        return e->ram_get(pos);
    } else {
        e->emulator_error(EE_INVALID_MEMORY_ACCESS);
        return 0;
    }
}

void emulator_memory_set(EmulatorCB* e, uint64_t pos, uint8_t data)
{
    // Write a memory byte to a *mapped* memory address.
#error Implement this function
    if (pos < e->machine_characteristics()->total_actual_memory) {
        e->ram_set(pos, data);
    } else {
        e->emulator_error(EE_INVALID_MEMORY_ACCESS);
    }
}