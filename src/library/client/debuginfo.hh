#ifndef DEBUGGING_INFO_HH_
#define DEBUGGING_INFO_HH_

#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

struct DebugInfo {

    struct Binary {
        std::vector<uint8_t> rom;
        uint64_t             load_pos;
    };

    struct SourceLine {
        uint64_t    address;
        std::string line;
        std::string comment;
    };

    bool                                            success;
    std::string                                     result_info;
    std::string                                     error_info;
    std::vector<Binary>                             binaries;
    std::vector<std::string>                        files;
    std::map<std::pair<size_t, size_t>, SourceLine> source_lines;
    std::unordered_map<std::string, uint64_t>       symbols;
    std::unordered_map<uint64_t, std::pair<size_t, size_t>> addresses;
    std::vector<std::string>                        files_to_watch;

    static constexpr uint64_t NO_ADDRESS = UINT64_MAX;
};

#endif