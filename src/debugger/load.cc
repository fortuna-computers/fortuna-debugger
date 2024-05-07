#include "load.hh"

#include <string>
#include <stdexcept>

#include <dlfcn.h>

extern "C" {
#include "user/compiler.h"
}

using namespace std::string_literals;

User user;

#define LOAD(f) { \
    *(void**)(&user.f) = dlsym(handle, #f); \
    if (!user.f) { \
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
    LOAD(compile)

    return;

err:
    dlclose(handle);
    throw std::runtime_error(error);
}