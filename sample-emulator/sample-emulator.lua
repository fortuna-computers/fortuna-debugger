return {

    --
    -- Machine definitions
    --
    machine = {

        -- Machine ID. Can by any 16-bit value, it's used to check if the debugger is
        -- connected to the right emulator. Needs to match the emulator id.
        id = 0x8f42,

        -- Machine name
        name = "Sample"

    },

    --
    -- Debugger definitions
    --

    debugger = {

        -- Source code comment separators. Used for syntax highlighting.
        comment_separators = ";#",

        -- List of memories. Usually just the RAM, but it can be used for things like
        -- a SDCard or a hard drive. The size is the full addressable memory space, and not
        -- just the physical size.
        memories = {
            { name = "RAM", size = 512 },
        },

        -- List of registers. The register count and order must match the list of register
        -- values that is returned by the emulator/computer.
        registers = {
            { name = "A", size = 8 },
        },

        -- List of boolean flags. The flag count and order must match the list of flags
        -- that is returned by the emulator/computer.
        flags = { "Z" },

        -- The terminal size
        terminal = { lines = 20, columns = 40 },

        -- These fields are used to setup the cycle interface - when we want to debug
        -- CPU cycles.
        --[[
        cycle_bytes = {
            { name = "ADDR", size = 16 },
            { name = "DATA", size = 8 },
        },
        cycle_bits = { "R", "XY" },
        ]]
    },

    --
    -- Options used to setup default microcontroller settings.
    --
    --[[
    microcontroller = {
        uart_baud_rate = 115200,   -- default UART baud rate
        vendor_id = "2e8a",        -- used to autodetect the USB serial port
        product_id = "000a",
    },
    ]]

    --
    -- Compiler. In this case we're compiling from Lua, but it can also be used to call
    -- an external compiler and then parse the result. The result needs to be in the
    -- following format:
    --
    -- {
    --     success = true/false,
    --     result_info = "Any relevant compiler output (not errors)",
    --     error = "Error - if the compilation was not successful",
    --     files = { "List", "of", "source", "files", "parsed" },
    --     source_lines = {
    --         { line = "Each source line", file_idx=1, line_number=1, address=0x0 },
    --         ...
    --     },
    --     symbols = {
    --         { name = "symbol", address = 0x40 }
    --         ...
    --     },
    --     binaries = {
    --        { rom = { 61, 62, 63 }, load_pos = 30 },
    --     },
    --     files_to_watch = { "List of files that the debugger will watch for changes" }
    -- }
    --
    compile = function(source_file)
        -- we're gonna fake a compilation here (see rom/echo.asm)
        return {
            success = true,
            result_info = "Compilation successful",
            files = { "echo.asm" },
            source_lines = {
                { line = "in          ; 00: 01        read input from keyboard and place it in A",      file_idx=1, line_number=1, address=0x0 },
                { line = "bz 0        ; 01: 02 00     if A == 0, jump to address 0 (start of program)", file_idx=1, line_number=2, address=0x1 },
                { line = "out         ; 03: 03        print whatever character is in A",                file_idx=1, line_number=3, address=0x3 },
                { line = "jp  0       ; 04: 04 00     jump to address 0 (start of program)",            file_idx=1, line_number=4, address=0x4 }
            },
            binaries = {
                { rom = { 0x1, 0x2, 0x0, 0x3, 0x4, 0x0 }, load_pos = 0 },
            },
        }
    end

}