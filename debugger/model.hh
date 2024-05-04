#ifndef MODEL_HH_
#define MODEL_HH_

#include <cstddef>
#include <cstdint>

struct DebuggerModel {
    struct Memory {
        size_t  pages;
        size_t  current_page;
        bool    data_present;
        uint8_t data[256];
    };

    Memory memory { .pages = 1, .current_page = 0, .data_present = false, .data = {0} };
};

#endif //MODEL_HH_
