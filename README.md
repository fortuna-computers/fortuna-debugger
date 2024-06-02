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
3. Download the repository submodules (`git submodule update --init --recursive`)
4. Build the project (`make`) - Linux and MacOS are supported
5. Install (`sudo make install`)
6. Run the debugger (`f-debugger`)
7. Load the sample emulator and play with it:
   ![image](https://github.com/fortuna-computers/fortuna-debugger/assets/84652/e9f5a49b-612b-4ad5-9047-ba9ae941dba8)
   ![image](https://github.com/fortuna-computers/fortuna-debugger/assets/84652/8b5baea0-77e5-412a-b2c2-aed93c552aec)

## Building your own computer

To build an **emulator**, copy the directory `sample-emulator/` and start from there.

To build a **computer**, the code will be similar to the emulator. The difference is that the code is going to run in a microcontroller, which will actually manipulate the CPU and memory instead of emulating them. The following files need to be part of the build:
- `src/library/server/libfdbg-server.*`
- `src/library/common/common.h`
- `src/library/protobuf/*.nanopb.*`

## Lua client

A client can be written in Lua to connect to the computer and automate some tasks, like unit testing the computer. This is the lua interface:

```lua
package.cpath = package.cpath .. ";/usr/local/lib/lua5.4/?.so"

local fdbg = require 'fdbg_client'
local client = fdbg.new()

-- client static methods
client.start_emulator("./emulator-executable")    -- returns emulator serial port

-- client object methods
client:load_user_definition("../my-computer.lua")
client:set_debugging_level("trace")
client:connect("/dev/serial-port", BAUD_RATE)    -- connect to computer


```
