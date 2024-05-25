#ifndef MODEL_HH_
#define MODEL_HH_

#include <cstddef>
#include <cstdint>

#include <deque>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "libfdbg-client.hh"
#include "config/config.hh"
#include "filewatcher/filewatcher.hh"
#include "terminalmodel.hh"

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
    void step(bool full);
    void add_breakpoint(uint64_t addr);
    void remove_breakpoint(uint64_t addr);
    void clear_breakpoints();
    void cycle();
    void clear_cycles() { cycles_.clear(); }
    void run(bool forever=false);
    void pause();
    void next();

    // getters

    std::optional<Upload> const&           upload_state() const { return upload_; }
    bool                                   connected() const { return connected_; }
    bool                                   debugging_session_started() const { return debugging_session_started_; }
    Config&                                config()  { return config_; }
    Machine const&                         machine() const { return client_.machine(); }
    DebugInfo const&                       debug() const { return debug_; }
    std::vector<const char*> const&        files_cstr() const { return files_cstr_; }
    std::vector<const char*> const&        symbols_cstr() const { return symbols_cstr_; }
    fdbg::ComputerStatus const&            computer_status() const { return computer_status_; }
    std::set<uint64_t> const&              breakpoints() const { return breakpoints_; }
    std::deque<fdbg::CycleResponse> const& cycles() const { return cycles_; }
    bool                                   running() const { return running_; }
    TerminalModel&                         terminal_model() { return terminal_model_; }

    std::optional<std::pair<size_t, size_t>> scroll_to_line_ {};

    std::string fmt_addr(uint64_t addr) const;

    void set_debugging_level(DebuggingLevel level);

private:
    FdbgClient                       client_;
    Config                           config_;
    DebugInfo                        debug_;
    std::optional<Upload>            upload_;
    bool                             connected_;
    bool                             debugging_session_started_ = false;
    uint8_t                          addr_sz_ = 0;
    fdbg::ComputerStatus             computer_status_;
    std::vector<const char*>         files_cstr_;
    std::vector<const char*>         symbols_cstr_;
    std::set<uint64_t>               breakpoints_;
    std::deque<fdbg::CycleResponse>  cycles_;
    bool                             running_ = false;
    FileWatcher                      file_watcher_;
    size_t                           update_count_ = 0;
    std::string                      source_file_;
    TerminalModel                    terminal_model_;

    void init_debugging_session();
    void scroll_to_pc();

    void set_running_state();
    void set_computer_status(fdbg::ComputerStatus const& computer_status);

    void do_event(fdbg::ComputerEvent const &event);
};

extern Model model;

#endif //MODEL_HH_
