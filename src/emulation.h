#ifndef EMULATION_H_
#define EMULATION_H_

#include <stdint.h>
#include "machine.h"

typedef enum EmulatorError {
    EE_INVALID_MEMORY_ACCESS,
} EmulatorError;

typedef struct EmulatorCB {
    MachineCharacteristics* (*machine_characteristics)();
    uint8_t                 (*ram_get)(uint32_t pos);
    void                    (*ram_set)(uint32_t pos, uint8_t data);
    void                    (*emulator_error)(EmulatorError error);
} EmulatorCB;

uint8_t emulator_memory_get(EmulatorCB* e, uint64_t pos);
void    emulator_memory_set(EmulatorCB* e, uint64_t pos, uint8_t data);

#endif //EMULATION_H_
