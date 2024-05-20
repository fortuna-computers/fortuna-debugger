return {

    machine = {
        id = 0x38f7,
        name = "CHIP-8",
        total_memory = 4 * 1024,
    },

    debugger = {
        comment_separators = ";#",
        registers = {
            { name = "A", size = 16 },
        },
        flags = { "EQ", "Z" },
        cycle_bytes = {
            { name = "ADDR", size = 16 },
            { name = "DATA", size = 8 },
        },
        cycle_bits = { "R", "XY" },
    },

    microcontroller = {
        uart_baud_rate = 115200,
        vendor_id = "2e8a",
        product_id = "000a",
    },

    compile = function(source_file)
        return {
            success = true,
            result_info = "Compilation successful",
            files = { "MAZE.SRC" },
            source_lines = {
                { line = "    nop  ; do nothing", file_idx=1, line_number=1, address=0x0 },
                { line = "hello:",  file_idx=1, line_number=2 },
                { line = "    nop", file_idx=1, line_number=3, address=0x1 },
                { line = "    nop", file_idx=1, line_number=4, address=0x2 },
                { line = "    nop", file_idx=1, line_number=5, address=0x3 },
                { line = "    nop", file_idx=1, line_number=6, address=0x4 },
                { line = "    nop", file_idx=1, line_number=7, address=0x5 },
                { line = "    nop", file_idx=1, line_number=8, address=0x6 },
                { line = "    nop", file_idx=1, line_number=9, address=0x7 },
                { line = "    nop", file_idx=1, line_number=10, address=0x8 },
                { line = "    nop", file_idx=1, line_number=11, address=0x9 },
                { line = "    nop;last one", file_idx=1, line_number=12, address=0xa },
            },
            symbols = {
                { name = "hello", address = 0x1 },
            },
            binaries = {
                { rom = { 61, 62, 63 }, load_pos = 30 },
            },
            files_to_watch = { "sample-chip8/rom/MAZE.SRC" },
        }
    end

}