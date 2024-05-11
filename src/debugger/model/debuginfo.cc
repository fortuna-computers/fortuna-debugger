#include "debuginfo.hh"

void DebugInfo::parse(CompilationResult const &cr)
{
    success = cr.success;
    error_info = cr.error_info ? cr.error_info : "";
    result_info = cr.result_info ? cr.result_info : "";

    for (size_t i = 0; i < cr.d_info.files_n; ++i)
        files.emplace_back(cr.d_info.files[i]);

    for (size_t i = 0; i < cr.d_info.source_lines_n; ++i) {
        ::SourceLine* sl = &cr.d_info.source_lines[i];
        source_lines[std::make_pair(sl->file_idx, sl->line_number)] = { sl->line, sl->address };
    }

    for (size_t i = 0; i < cr.d_info.symbols_n; ++i) {
        Symbol* sym = &cr.d_info.symbols[i];
        symbols[sym->name] = sym->address;
    }

    for (size_t i = 0; i < cr.binaries_n; ++i) {
        Binary binary;
        binary.rom.reserve(cr.binaries[i].rom_sz);
        for (size_t j = 0; j < cr.binaries[i].rom_sz; ++j)
            binary.rom.push_back(cr.binaries[i].rom[j]);
        binary.load_pos = cr.binaries[i].load_pos;
        binaries.emplace_back(std::move(binary));
    }
}

void DebugInfo::free_(CompilationResult const& cr)
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
