#ifndef LOAD_HH_
#define LOAD_HH_

#include <cstdint>
#include <functional>

extern uint16_t (*machine_id)();

void load_machine(const char* filename);

#endif //LOAD_HH_
