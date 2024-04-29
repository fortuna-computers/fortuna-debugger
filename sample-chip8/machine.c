#include "machine.h"

MachineCharacteristics* machine_characteristics()
{
    static MachineCharacteristics mc = {
            .id = 0x8f42,
            .total_actual_memory = 4096,
            .total_mappable_memory = 4096,
    };
    return &mc;
}