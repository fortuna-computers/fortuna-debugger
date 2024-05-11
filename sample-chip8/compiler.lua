return function(source_file)
    return {
        success = true,
        binaries = {
            { rom = { 61, 62, 63 }, load_pos = 30 },
        }
    }
    --[[
    return {
        success = false,
        error_info = "There was a compilation error (from Lua)",
    }
    ]]
end
