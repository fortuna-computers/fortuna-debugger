#ifndef DEBUGGING_INFO_HH_
#define DEBUGGING_INFO_HH_

#include "user/compiler.h"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

struct DebugInfo {

    struct Binary {
        std::vector<uint8_t> rom;
        uint64_t             load_pos;
    };

    struct SourceLine {
        std::string line;
        uint64_t    address;
    };

    bool                                            success;
    std::string                                     result_info;
    std::string                                     error_info;
    std::vector<Binary>                             binaries;
    std::vector<std::string>                        files;
    std::map<std::pair<size_t, size_t>, SourceLine> source_lines;
    std::map<std::string, uint64_t>                 symbols;

    void parse_and_free(CompilationResult const& cr) {
        this->parse(cr);
        this->free_(cr);
    }

private:
    void parse(CompilationResult const& cr);
    void free_(CompilationResult const& cr);
};

#endif