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

    void load_machine(std::string const& file);

    void connect_to_emulator(std::string const& path);
    void connect_to_serial_port(std::string const& serial_port, uint32_t baud_rate);

    void update();

    void init_debugging_session();
    void initialize_memory();
    void change_memory_page(int64_t page);

    void compile(std::string const& source_file);

    std::optional<Upload> const &upload_state() const { return upload_; }
    bool connected() const { return connected_; }
    bool debugging_session_started() const { return debugging_session_started_; }

    std::string fmt_addr(uint64_t addr) const;

    Config&        config()  { return config_; }
    Machine const& machine() const { return client_.machine(); }

private:
    FdbgClient            client_;
    Config                config_;
    DebugInfo             debug_;
    std::optional<Upload> upload_;
    bool                  connected_;
    bool                  debugging_session_started_ = false;
    uint8_t               addr_sz_ = 0;
};

extern Model model;

#endif //MODEL_HH_
