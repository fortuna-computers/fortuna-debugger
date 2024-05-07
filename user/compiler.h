#ifndef COMPILER_H_
#define COMPILER_H_

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define NO_ADDRESS UINT64_MAX;

typedef struct Binary {
    uint8_t* rom;
    size_t   rom_sz;
    uint64_t load_pos;
} Binary;

typedef struct Location {
    size_t   file;
    size_t   line;
    uint64_t address;
} Location;

typedef struct Symbol {
    char*  name;
    size_t address;
} Symbol;

typedef struct SourceLine {
    char*    line;
    Location location;
} SourceLine;

typedef struct DebuggingInfo {
    char**      files;
    size_t      files_n;
    SourceLine* source_lines;
    size_t      source_lines_n;
    Symbol*     symbols;
    size_t      symbols_n;
} DebuggingInfo;

typedef struct CompilationResult {
    bool          success;
    char*         result_info;
    char*         error_info;

    DebuggingInfo d_info;

    Binary*       binaries;
    size_t        binaries_n;
} CompilationResult;

CompilationResult compile(const char* source_file);

#endif