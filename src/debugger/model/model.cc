#include "model.hh"

#include "load/load.hh"

DebuggerModel::~DebuggerModel()
{
    emulator_.kill();
}

void DebuggerModel::connect_to_emulator()
{
    std::string port = emulator_.init();

    emulator_.run_as_thread();

    connect_to_serial_port(port, EMULATOR_BAUD_RATE);

    if (!debug_.binaries.empty()) {
        upload_ = Upload {
                .binary_idx = 0,
                .binary_count = debug_.binaries.size(),
                .current = 0,
                .total = debug_.binaries.front().rom.size(),
                .address = debug_.binaries.front().load_pos,
                .verifying = false,
        };
    }
}

void DebuggerModel::connect_to_serial_port(const std::string &serial_port, uint32_t baud_rate)
{
    client_.connect(serial_port, baud_rate);
    connected_ = true;
    client_.set_debugging_level(DebuggingLevel::TRACE);
    client_.ack(user.machine_id());
}

void DebuggerModel::update()
{
}

void DebuggerModel::initialize_memory()
{
    memory.pages = user.total_mappable_memory() / PAGE_SZ;
    change_memory_page(0);
}

void DebuggerModel::change_memory_page(int64_t page)
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

void DebuggerModel::compile(std::string const& source_file)
{
    CompilationResult cr = user.compile(source_file.c_str());
    debug_.parse_and_free(cr);

    if (!debug_.success)
        throw std::runtime_error(debug_.error_info);
}