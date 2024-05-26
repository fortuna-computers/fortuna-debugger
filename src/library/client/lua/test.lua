local fdbg_client = require 'fdbg_client'

local client = fdbg_client.new()
client:load_user_definition('../../sample-chip8/sample-chip8.lua')

local port = client.start_emulator('../../sample-chip8/chip8-emulator')
print(port)

client:connect(port)

client:ack(0x38f7)

local data = client:read_memory(0, 0, 32)
for k,v in ipairs(data) do print(v) end