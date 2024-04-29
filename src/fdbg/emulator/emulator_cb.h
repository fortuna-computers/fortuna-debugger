#ifndef EMULATOR_HH_
#define EMULATOR_HH_

#include <stdint.h>

#include "../machine.h"

typedef enum EmulatorError {
    EE_INVALID_MEMORY_ACCESS,
} EmulatorError;

typedef struct EmulatorCB {
    MachineCharacteristics* (*machine_characteristics)();
    uint8_t                 (*ram_get)(uint32_t pos);
    void                    (*ram_set)(uint32_t pos, uint8_t data);
    void                    (*emulator_error)(EmulatorError error);
} EmulatorCB;

#endif //EMULATOR_HH_
