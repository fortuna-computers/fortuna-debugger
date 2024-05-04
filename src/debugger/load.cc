#include "load.hh"

#include <string>
#include <stdexcept>

#include <dlfcn.h>

using namespace std::string_literals;

uint16_t (*machine_id)();
const char* (*machine_name)();

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

    return;

err:
    dlclose(handle);
    throw std::runtime_error(error);
}