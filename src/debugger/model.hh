#ifndef MODEL_HH_
#define MODEL_HH_

#include <cstddef>
#include <cstdint>

#include <optional>

#include "libfdbg-client.hh"
#include "emulator/emulator.hh"

class DebuggerModel {
    static constexpr size_t PAGE_SZ = 256;

public:
    ~DebuggerModel();

    struct Memory {
        size_t                 pages;
        size_t                 current_page;
        std::optional<uint8_t> data[PAGE_SZ];
    };

    Memory memory { .pages = 1, .current_page = 0, .data = {{}} };

    void connect_to_emulator();
    void connect_to_serial_port(std::string const& serial_port, uint32_t baud_rate);

    void initialize_memory();
    void change_memory_page(int64_t page);

    void compile(std::string const& source_file);

private:
    FdbgClient client_;
    Emulator   emulator_;
    bool       connected_ = false;

    void free_compilation_result(struct CompilationResult& cr);
};

#endif //MODEL_HH_
