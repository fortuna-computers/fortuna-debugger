#include "model.hh"

#include "ui/ui.hh"
#include "ui/keypress.hh"

Model::Model()
{
    config_.load();
    client_.set_on_debug([this](std::string const& text) { debug_text_ = text + "\n" + debug_text_; });
}

void Model::load_machine(std::string const& file)
{
    client_.load_user_definition(file);
    client_.set_debugging_level((DebuggingLevel) config_.get_int("debugging_level"));

    terminal_model_.initialize(machine().terminal_columns, machine().terminal_lines);
}

void Model::start_emulator(std::string const& path)
{
    std::string port = FdbgClient::start_emulator(path);
    connect_to_serial_port(port, EMULATOR_BAUD_RATE);
}

void Model::connect_to_running_emulator(int pid)
{
    std::string port = FdbgClient::get_emulator_port((pid_t) pid);
    connect_to_serial_port(port, EMULATOR_BAUD_RATE);
}

void Model::connect_to_serial_port(const std::string &serial_port, uint32_t baud_rate)
{
    client_.connect(serial_port, baud_rate);
    connected_ = true;
    auto ack = client_.ack(machine().id);
    printf("Connection acknowledged. Memory use: %d bytes from server, %d bytes for ToComputer buffer, %d bytes for ToDebugger buffer.\n",
           ack.server_sz(), ack.to_computer_sz(), ack.to_debugger_sz());
}

void Model::upload_rom_and_start()
{
    if (!debug_.binaries.empty()) {
        upload_ = Upload {
                .binary_idx = 0,
                .binary_count = debug_.binaries.size(),
                .current = 0,
                .total = debug_.binaries.front().rom.size(),
                .address = debug_.binaries.front().load_pos,
                .verify = config_.get_bool("verify_upload_rom"),
                .upload_context = {}
        };
    } else {
        init_debugging_session();
    }
}

void Model::init_debugging_session()
{
    set_computer_status(client_.reset());

    size_t i = 0;
    for (auto const& memory: machine().memories) {
        memories.push_back({ .pages = memory.size / PAGE_SZ, .current_page = 0, .data = {{}} });
        change_memory_page(i++, 0);
    }

    debugging_session_started_ = true;
    ui.start_debugging_session();
}

void Model::update()
{
    if (upload_) {
        auto const& binary = debug_.binaries[upload_->binary_idx];
        bool in_progress = client_.write_memory_step(0, upload_->address, binary.rom, upload_->upload_context, upload_->verify);
        if (in_progress) {
            upload_->current += MAX_MEMORY_TRANSFER;
        } else {
            ++upload_->binary_idx;
            if (upload_->binary_idx >= upload_->binary_count) { // upload complete
                upload_ = {};
                init_debugging_session();
                return;
            }
            auto const& new_binary = debug_.binaries[upload_->binary_idx];
            upload_->current = 0;
            upload_->total = new_binary.rom.size();
            upload_->address = new_binary.load_pos;
        }
    }

    if (running_) {
        auto cr = client_.run_status(get_user_events());
        running_ = cr.running();
        for (int i = 0; i < cr.events_size(); ++i)
            do_event(cr.events(i));
        computer_status_.set_pc(cr.pc());
        if (!running_)
            scroll_to_pc();
    }

    if (update_count_ % 40 == 0)  // every ~600ms
        file_watcher_.run_verify();

    if (file_watcher_.updated()) {
        compile(source_file_);
        file_watcher_.reset();
        upload_rom_and_start();
    }

    ++update_count_;
}

void Model::change_memory_page(uint8_t nr, int64_t page)
{
    auto& memory = memories.at(nr);

    if (page >= (int64_t) memory.pages)
        page = 0;
    if (page < 0)
        page = ((int64_t) memory.pages) - 1;

    for (auto& byte : memory.data)
        byte = {};

    memory.current_page = page;

    for (size_t i = 0; i < (PAGE_SZ / 64); ++i) {
        auto bytes = client_.read_memory(nr, (page + i) * 64, 64, 4);
        std::copy(std::begin(bytes), std::end(bytes), std::begin(memory.data) + (i * 64));
    }
}

void Model::reset()
{
    set_computer_status(client_.reset());
    scroll_to_pc();
}

void Model::step(bool full)
{
    fdbg::ComputerStatus computer_status = client_.step(full, get_user_events());
    for (int i = 0; i < computer_status.events_size(); ++i)
        do_event(computer_status.events(i));
    set_computer_status(computer_status);
    scroll_to_pc();
}

void Model::cycle()
{
    // TODO - check response size vs machine size
    auto cycle_response = client_.cycle();

    if (cycle_response.bytes_size() != 0 && (size_t) cycle_response.bytes_size() != machine().cycle_bytes.size())
        throw std::runtime_error("The number of bytes sent by the computer doesn't match the number of bytes in the machine definition.");
    if (cycle_response.bits_size() != 0 && (size_t) cycle_response.bits_size() != machine().cycle_bits.size())
        throw std::runtime_error("The number of bits sent by the computer doesn't match the number of bits in the machine definition.");

    if (cycle_response.pc())
        computer_status_.set_pc(cycle_response.pc());
    cycles_.push_front(cycle_response);
    scroll_to_pc();
}

void Model::compile(std::string const& source_file)
{
    debug_ = client_.machine().compile(source_file);
    if (!debug_.success)
        throw std::runtime_error(debug_.error_info);

    for (auto const& file: debug_.files)
        files_cstr_.push_back(file.c_str());
    for (auto const& symbol: debug_.symbols)
        symbols_cstr_.push_back(symbol.first.c_str());
    std::sort(symbols_cstr_.begin(), symbols_cstr_.end(), [](const char* a, const char* b) { return std::string(a) < std::string(b); });
    symbols_cstr_.insert(symbols_cstr_.begin(), "");

    scroll_to_pc();

    if (machine().memories.at(0).size <= 0xffff)
        addr_sz_ = 4;
    else if (machine().memories.at(0).size <= 0xffffff)
        addr_sz_ = 6;
    else
        addr_sz_ = 8;

    file_watcher_.update_files(debug_.files_to_watch);

    source_file_ = source_file;
}

std::string Model::fmt_addr(uint64_t addr) const
{
    char buf[9] = {0};
    snprintf(buf, sizeof buf, "%0*llX", addr_sz_, addr);
    return buf;
}

void Model::add_breakpoint(uint64_t addr)
{
    breakpoints_ = client_.add_breakpoint(addr);
}

void Model::remove_breakpoint(uint64_t addr)
{
    breakpoints_ = client_.remove_breakpoint(addr);
}

void Model::clear_breakpoints()
{
    breakpoints_.clear();
    client_.clear_breakpoints();
}

void Model::run(bool forever)
{
    client_.run(forever);
    set_running_state();
}

void Model::set_running_state()
{
    running_ = true;
    computer_status_.clear_flags();
    computer_status_.clear_stack();
    computer_status_.clear_registers();
}

void Model::next()
{
    auto nx = client_.next();
    if (nx.running()) {
        set_running_state();
    } else {
        computer_status_.set_pc(nx.pc());
        scroll_to_pc();
    }
}

void Model::pause()
{
    computer_status_ = client_.pause();
    running_ = false;
    scroll_to_pc();
}

void Model::scroll_to_pc()
{
    try {
        scroll_to_line_ = debug_.addresses.at(computer_status_.pc());
    } catch (std::out_of_range&) {}
}

void Model::set_computer_status(fdbg::ComputerStatus const &computer_status)
{
    if (computer_status.registers_size() != 0 && (size_t) computer_status.registers_size() != machine().registers.size())
        throw std::runtime_error("The number of register sent by the computer doesn't match the number of registers in the machine definition.");
    if (computer_status.flags_size() != 0 && (size_t) computer_status.flags_size() != machine().flags.size())
        throw std::runtime_error("The number of flags sent by the computer doesn't match the number of flags in the machine definition.");
    computer_status_ = computer_status;
}

void Model::set_debugging_level(DebuggingLevel level)
{
    client_.set_debugging_level(level);
}

void Model::do_event(fdbg::ComputerEvent const &event)
{
    switch (event.type_case()) {
        case fdbg::ComputerEvent::kTerminalPrint:
            terminal_model_.add_string(event.terminal_print().text());
            break;
        case fdbg::ComputerEvent::TYPE_NOT_SET:
            throw std::runtime_error("Missing event type");
    }
}

std::vector<fdbg::UserEvent> Model::get_user_events()
{
    std::vector<fdbg::UserEvent> events;

    if (terminal_model_.next_tx) {
        fdbg::UserEvent event;
        auto terminal_keypress = new fdbg::UserEvent::TerminalKeypress();
        terminal_keypress->set_text(*terminal_model_.next_tx);
        event.set_allocated_terminal_keypress(terminal_keypress);
        events.push_back(std::move(event));

        terminal_model_.next_tx = {};
    }

    if (running_) {
        auto key = check_for_keypress();
        if (key) {
            fdbg::UserEvent event;
            auto terminal_keypress = new fdbg::UserEvent::TerminalKeypress();
            terminal_keypress->set_text(*key);
            event.set_allocated_terminal_keypress(terminal_keypress);
            events.push_back(std::move(event));
        }
    }

    return events;
}

void Model::interrupt(uint64_t number)
{
    client_.interrupt(number);
}
