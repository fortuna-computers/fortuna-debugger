return function(source_file)
    local result = {
        success = true,
        result_info = "Compilation successful",
        files = { "MAZE.SRC" },
        source_lines = {
            { line = "    nop", file_idx=1, line_number=1, address=0x0 },
            { line = "hello:",  file_idx=1, line_number=2 },
            { line = "    nop", file_idx=1, line_number=3, address=0x1 },
        },
        symbols = {
            { name = "hello", address = 0x1 },
        },
        binaries = {
            { rom = { 61, 62, 63 }, load_pos = 30 },
        }
    }
    print(result.files)
    return result
end
