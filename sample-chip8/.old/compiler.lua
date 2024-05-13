return function(source_file)
    return {
        success = true,
        result_info = "Compilation successful",
        d_info = {
            files = { "MAZE.SRC" },
            source_lines = {
                { line = "    nop", file_idx=1, line_number=1, address=0x0 },
                { line = "hello:",  file_idx=1, line_number=2 },
                { line = "    nop", file_idx=1, line_number=3, address=0x1 },
            },
            symbols = {
                { name = "hello", address = 0x1 },
            },
        },
        binaries = {
            { rom = { 61, 62, 63 }, load_pos = 30 },
        }
    }
end
