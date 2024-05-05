#ifndef LOAD_HH_
#define LOAD_HH_

#include <cstdint>
#include <functional>

void load_machine(const char* filename);

extern uint16_t    (*machine_id)();
extern const char* (*machine_name)();

extern uint64_t    (*total_mappable_memory)();

extern uint32_t    (*uart_baud_rate)();
extern const char* (*microcontroller_vendor_id)();
extern const char* (*microcontroller_product_id)();

extern void        (*emulator_init)();
extern void        (*emulator_reset)();

extern void        (*emulator_ram_set)(uint64_t pos, uint8_t data);
extern uint8_t     (*emulator_ram_get)(uint32_t pos);

#endif //LOAD_HH_
