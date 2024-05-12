#ifndef MODEL_HH_
#define MODEL_HH_

#include <cstddef>
#include <cstdint>

#include <optional>

#include "libfdbg-client.hh"
#include "emulator/emulator.hh"
#include "model/debuginfo.hh"

class DebuggerModel {
    static constexpr size_t PAGE_SZ = 256;

public:
    ~DebuggerModel();

    struct Memory {
        size_t                 pages;
        size_t                 current_page;
        std::optional<uint8_t> data[PAGE_SZ];
    };

    struct Upload {
        size_t   binary_idx;
        size_t   binary_count;
        size_t   current;
        size_t   total;
        uint64_t address;
        bool   verifying;
    };

    Memory memory { .pages = 1, .current_page = 0, .data = {{}} };

    void connect_to_emulator();
    void connect_to_serial_port(std::string const& serial_port, uint32_t baud_rate);

    void update();

    void initialize_memory();
    void change_memory_page(int64_t page);

    void compile(std::string const& source_file);

    std::optional<Upload> const &upload_state() const { return upload_; }
    bool connected() const { return connected_; }

    std::string fmt_addr(uint64_t addr) const;

private:
    FdbgClient            client_;
    Emulator              emulator_;
    DebugInfo             debug_;
    std::optional<Upload> upload_;
    bool                  connected_;
    uint8_t               addr_sz_ = 0;
};

#endif //MODEL_HH_
