#include "model.hh"

#include "load.hh"

DebuggerModel::~DebuggerModel()
{
    emulator_.kill();
}

void DebuggerModel::connect_to_emulator()
{
    std::string port = emulator_.init();

    emulator_.run_as_thread();

    connect_to_serial_port(port, EMULATOR_BAUD_RATE);
}

void DebuggerModel::connect_to_serial_port(const std::string &serial_port, uint32_t baud_rate)
{
    client_.connect(serial_port, baud_rate);
    connected_ = true;
    client_.set_debugging_level(DebuggingLevel::TRACE);
    client_.ack(user.machine_id());
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
    if (!cr.success) {
        std::string error = cr.error_info;
        free_compilation_result(cr);
        throw std::runtime_error(error);
    }

    free_compilation_result(cr);
}

void DebuggerModel::free_compilation_result(CompilationResult& cr)
{
    free(cr.result_info);
    free(cr.error_info);

    for (size_t i = 0; i < cr.d_info.files_n; ++i)
        free(cr.d_info.files[i]);
    free(cr.d_info.files);

    for (size_t i = 0; i < cr.d_info.source_lines_n; ++i)
        free(cr.d_info.source_lines[i].line);
    free(cr.d_info.source_lines);

    for (size_t i = 0; i < cr.d_info.symbols_n; ++i)
        free(cr.d_info.symbols[i].name);
    free(cr.d_info.symbols);

    for (size_t i = 0; i < cr.binaries_n; ++i)
        free(cr.binaries[i].rom);
    free(cr.binaries);
}