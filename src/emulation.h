#ifndef EMULATION_H_
#define EMULATION_H_

#include <stdint.h>
#include "fdbg/emulator/emulator_cb.h"

uint8_t emulator_memory_get(EmulatorCB* e, uint64_t pos);
void    emulator_memory_set(EmulatorCB* e, uint64_t pos, uint8_t data);

#endif //EMULATION_H_
