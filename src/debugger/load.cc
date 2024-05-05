#include "load.hh"

#include <string>
#include <stdexcept>

#include <dlfcn.h>

using namespace std::string_literals;

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

#define LOAD(f) { \
    *(void**)(&f) = dlsym(handle, #f); \
    if (!f) { \
        error = "Error linking function `" #f "`: "s + dlerror(); \
        goto err; \
    } \
}

void load_machine(const char* filename)
{
    std::string error;
    dlerror();

    void* handle = dlopen(filename, RTLD_NOW);
    if (!handle) {
        error = "Error loading machine file: "s + dlerror();
        goto err;
    }

    LOAD(machine_id)
    LOAD(machine_name)
    LOAD(total_mappable_memory)
    LOAD(uart_baud_rate)
    LOAD(microcontroller_vendor_id)
    LOAD(microcontroller_product_id)
    LOAD(emulator_init)
    LOAD(emulator_reset)
    LOAD(emulator_ram_get)
    LOAD(emulator_ram_set)

    return;

err:
    dlclose(handle);
    throw std::runtime_error(error);
}