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
    }
}

void Model::connect_to_serial_port(const std::string &serial_port, uint32_t baud_rate)
{
    client_.connect(serial_port, baud_rate);
    connected_ = true;
    client_.set_debugging_level(DebuggingLevel::TRACE);
    client_.ack(machine().id);
}

void Model::init_debugging_session()
{
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
            if (upload_->binary_idx >= upload_->binary_count) {
                upload_ = {};   // upload complete
                return;
            }
            auto const& new_binary = debug_.binaries[upload_->binary_idx];
            upload_->current = 0;
            upload_->total = new_binary.rom.size();
            upload_->address = new_binary.load_pos;
        }
    }
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

void Model::compile(std::string const& source_file)
{
    debug_ = client_.machine().compile(source_file);

    if (machine().total_memory <= 0xffff)
        addr_sz_ = 4;
    else if (machine().total_memory <= 0xffffff)
        addr_sz_ = 6;
    else
        addr_sz_ = 8;
}

std::string Model::fmt_addr(uint64_t addr) const
{
    char buf[9] = {0};
    snprintf(buf, sizeof buf, "%0*llX", addr_sz_, addr);
    return buf;
}