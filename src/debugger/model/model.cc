#include "model.hh"

#include "ui/ui.hh"

Model::Model()
{
    config_.load();
}

void Model::load_machine(std::string const& file)
{
    client_.load_user_definition(file);
}

void Model::connect_to_emulator(std::string const& path)
{
    std::string port = FdbgClient::start_emulator(path);
    connect_to_serial_port(port, EMULATOR_BAUD_RATE);
}

void Model::connect_to_serial_port(const std::string &serial_port, uint32_t baud_rate)
{
    client_.connect(serial_port, baud_rate);
    connected_ = true;
    client_.set_debugging_level(DebuggingLevel::TRACE);
    client_.ack(machine().id);
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

    memory.pages = machine().total_memory / PAGE_SZ;
    change_memory_page(0);

    debugging_session_started_ = true;
    ui.start_debugging_session();
}

void Model::update()
{
    if (upload_) {
        auto const& binary = debug_.binaries[upload_->binary_idx];
        bool in_progress = client_.write_memory_step(upload_->address, binary.rom, upload_->upload_context, upload_->verify);
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
        auto cr = client_.run_status();
        running_ = cr.running();
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

void Model::change_memory_page(int64_t page)
{
    if (page >= (int64_t) memory.pages)
        page = 0;
    if (page < 0)
        page = ((int64_t) memory.pages) - 1;

    for (auto& byte : memory.data)
        byte = {};

    memory.current_page = page;

    for (size_t i = 0; i < (PAGE_SZ / 64); ++i) {
        auto bytes = client_.read_memory((page + i) * 64, 64, 4);
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
    set_computer_status(client_.step(full));
    scroll_to_pc();
}

void Model::cycle()
{
    // TODO - check response size vs machine size
    auto cycle_response = client_.cycle();
    if (cycle_response.pc())
        computer_status_.set_pc(cycle_response.pc());
    cycles_.push_front(cycle_response);
    scroll_to_pc();
}

void Model::compile(std::string const& source_file)
{
    debug_ = client_.machine().compile(source_file);

    for (auto const& file: debug_.files)
        files_cstr_.push_back(file.c_str());
    for (auto const& symbol: debug_.symbols)
        symbols_cstr_.push_back(symbol.first.c_str());
    std::sort(symbols_cstr_.begin(), symbols_cstr_.end(), [](const char* a, const char* b) { return std::string(a) < std::string(b); });
    symbols_cstr_.insert(symbols_cstr_.begin(), "");

    if (machine().total_memory <= 0xffff)
        addr_sz_ = 4;
    else if (machine().total_memory <= 0xffffff)
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
    client_.next();
    set_running_state();
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
    if (computer_status.registers_size() != 0 && computer_status.registers_size() != machine().registers.size())
        throw std::runtime_error("The number of register sent by the computer doesn't match the number of registers in the machine definition.");
    if (computer_status.flags_size() != 0 && computer_status.flags_size() != machine().flags.size())
        throw std::runtime_error("The number of flags sent by the computer doesn't match the number of flags in the machine definition.");
    computer_status_ = computer_status;
}
