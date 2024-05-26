local fdbg_client = require 'fdbg_client'
local client = fdbg_client.new()
client:load_user_definition('../../sample-chip8/sample-chip8.lua')
print(client.start_emulator('../../sample-chip8/chip8-emulator'))