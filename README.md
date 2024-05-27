# fortuna-debugger

***Project in progress! Not ready for use.***

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

1. Install the dependencies ([Lua 5.4](https://www.lua.org/), [GLFW 3](https://www.glfw.org/) and [Protobuf](https://protobuf.dev/).
2. Clone the repository
3. Download the repository submodules (`git submodule update --init --recursive`)
4. Build the project (`make`) - Linux and MacOS are supported
5. Install (`sudo make install`)
6. Run the debugger (`f-debugger`)
7. Load the sample emulator and play with it:
