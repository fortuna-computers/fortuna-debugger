#ifndef EMULATOR_H_
#define EMULATOR_H_

#include <stdint.h>

void    emulator_init();
void    emulator_reset();

void    emulator_ram_set(uint64_t pos, uint8_t data);
uint8_t emulator_ram_get(uint32_t pos);

#endif //EMULATOR_H_
