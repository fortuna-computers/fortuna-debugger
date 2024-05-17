#ifndef MODEL_HH_
#define MODEL_HH_

#include <cstddef>
#include <cstdint>

#include <optional>

#include "libfdbg-client.hh"
#include "config/config.hh"

class Model {
    static constexpr size_t PAGE_SZ = 256;

public:
    Model();

    struct Memory {
        size_t                 pages;
        size_t                 current_page;
        std::optional<uint8_t> data[PAGE_SZ];
    };

    struct Upload {
        size_t             binary_idx;
        size_t             binary_count;
        size_t             current;
        size_t             total;
        uint64_t           address;
        bool               verify;
        FdbgClient::Upload upload_context;
    };

    Memory memory { .pages = 1, .current_page = 0, .data = {{}} };

    // initialization

    void load_machine(std::string const& file);

    void connect_to_emulator(std::string const& path);
    void connect_to_serial_port(std::string const& serial_port, uint32_t baud_rate);

    void compile(std::string const& source_file);

    void upload_rom_and_start();

    // update

    void update();

    // user events

    void change_memory_page(int64_t page);
    void reset();

    // getters

    std::optional<Upload> const& upload_state() const { return upload_; }
    bool                         connected() const { return connected_; }
    bool                         debugging_session_started() const { return debugging_session_started_; }
    Config&                      config()  { return config_; }
    Machine const&               machine() const { return client_.machine(); }
    DebugInfo const&             debug() const { return debug_; }
    std::vector<const char*> const& files_cstr() const { return files_cstr_; }
    fdbg::ComputerStatus const&  computer_status() const { return computer_status_; }

    std::string fmt_addr(uint64_t addr) const;

private:
    FdbgClient               client_;
    Config                   config_;
    DebugInfo                debug_;
    std::optional<Upload>    upload_;
    bool                     connected_;
    bool                     debugging_session_started_ = false;
    uint8_t                  addr_sz_ = 0;
    fdbg::ComputerStatus     computer_status_;
    std::vector<const char*> files_cstr_;

    void init_debugging_session();
};

extern Model model;

#endif //MODEL_HH_
