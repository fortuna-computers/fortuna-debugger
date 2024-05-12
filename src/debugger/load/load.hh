#ifndef LOAD_HH_
#define LOAD_HH_

#include <cstdint>
#include <functional>

extern "C" {
#include "user/compiler.h"
}

void load_machine(const char* filename);

struct User {
    uint16_t    (*machine_id)();
    const char* (*machine_name)();

    uint64_t    (*total_mappable_memory)();

    uint32_t    (*uart_baud_rate)();
    const char* (*microcontroller_vendor_id)();
    const char* (*microcontroller_product_id)();

    void        (*emulator_init)();
    void        (*emulator_reset)();

    void        (*emulator_ram_set)(uint64_t pos, uint8_t data);
    uint8_t     (*emulator_ram_get)(uint32_t pos);

    CompilationResult (*compile)(const char* source_file);
};

extern User user;

#endif //LOAD_HH_
