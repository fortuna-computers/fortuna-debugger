return {

    machine = {
        id = 0x38f7,
        name = "CHIP-8",
        total_memory = 4 * 1024,
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

}