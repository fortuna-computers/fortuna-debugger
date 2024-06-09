# fortuna-debugger

Fortuna Debugger is a set of tools for computer designers that, together, provide debugging capability to any custom computer.

The user needs to provide custom code that will:

* Describe the computer (CPU, registers, etc)
* Run a compiler and interpret its results
* Emulate the computer
* Execute operations in a microcontroller installed in the computer, to obtain the state of the computer (registers, program counter, memory, etc).

Once the above is provided by the user, the following tools are made available to the user:

* A **debugger** that:
  * Connects to the computer firmware (via serial) and provide **onboard real-time debugging ability**
  * Connects to an **emulator** to facilitate software development
* A **library** (`libfdbg-client.so` for C and `fdbg_client.so` for Lua) that allows the user to write its own programs to run onboard (like test suites, for example).

## Building and running

1. Install the dependencies ([Lua 5.4](https://www.lua.org/), [GLFW 3](https://www.glfw.org/) and [Protobuf](https://protobuf.dev/)).
2. Clone the repository
3. Build the project (`make`) - only Linux and MacOS are supported
4. Run the debugger (`f-debugger`)

## Building your own computer

Look at https://github.com/fortuna-computers/fortuna-5 for a sample emulator and computer.

## Lua client

A client can be written in Lua to connect to the computer and automate some tasks, like unit testing the computer. This is the lua interface:

```lua
package.cpath = package.cpath .. ";/usr/local/lib/lua5.4/?.so"

local fdbg = require 'fdbg_client'
local client = fdbg.new()

-- static methods
client.start_emulator("./emulator-executable")    -- returns emulator serial port

-- setup/connection
client:load_user_definition("../my-computer.lua")
client:set_debugging_level("trace")
client:autodetect_usb_serial_port()              -- return auto-detected serial port
client:connect("/dev/serial-port", BAUD_RATE)    -- connect to computer

-- commands
client:ack(id)    -- return AckResponse { id, server_sz, to_computer_sz, to_debugger_sz }
client:reset()
client:write_memory(nr, pos, data_tbl, verify=false)
client:read_memory(nr, pos, sz, sequences=1)    -- return byte array
client:step(full=false)  -- return ComputerStatus { pc, registers={}, flags={}, stack={} }
client:run(forever=false)
client:run_status()  -- return RunStatus { running, pc }
client:pause()       -- return RunStatus
client:add_breakpoint(bkp)      -- return breakpoint list
client:remove_breakpoint(bkp)   -- return breakpoint list

-- high level calls
client:compile_and_write_memory(source_filenamne, nr=0, validate=false)
```
