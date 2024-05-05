#ifndef MODEL_HH_
#define MODEL_HH_

#include <cstddef>
#include <cstdint>

#include "libfdbg-client.hh"
#include "debugger/emulator/emulator.hh"

class DebuggerModel {
public:
    ~DebuggerModel();

    struct Memory {
        size_t  pages;
        size_t  current_page;
        bool    data_present;
        uint8_t data[256];
    };

    Memory memory { .pages = 1, .current_page = 0, .data_present = false, .data = {0} };
    // TODO - calculate number of pages

    void connect_to_emulator();
    void connect_to_serial_port(std::string const& serial_port, uint32_t baud_rate);

    void change_memory_page(int64_t page);

    void update();

private:
    FdbgClient client_;
    Emulator   emulator_;
    bool       connected_ = false;
};

#endif //MODEL_HH_
