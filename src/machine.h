#ifndef MACHINE_H_
#define MACHINE_H_

#include <stdint.h>

typedef struct MachineCharacteristics {
    uint16_t id;                     // Id that uniquely identifies the machine. This is used by the debugger
                                     //    to check if the debugger is connected to the right device.
    uint64_t total_mappable_memory;  // Total memory that is theoretically accessible by the CPU. Memory access
                                     //    to these addresses should never fail.
    uint32_t total_actual_memory;    // Total RAM memory. Used by the emulator to allocate the memory on the PC.
} MachineCharacteristics;

MachineCharacteristics* machine_characteristics();

#endif //MACHINE_H_
