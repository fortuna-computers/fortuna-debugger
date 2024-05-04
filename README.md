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
* A **library** (`libfdbg.so`) that allows the user to write its own programs to run onboard (like test suites, for example).

A sample implementation is provided for running an emulator version of the [CHIP-8](https://chip-8.github.io/) on a Raspberry Pi Pico.

## Implementing a new architecture

1. Fork the repository, or add as a submodule in another project.
2. Update the following files:
     - [user/machine.h](user/machine.h): machine characteristics
     - [user/emulator.c](user/emulator.c): emulator code
     - [user/compiler.lua](user/compiler.lua): compilation code (call the compiler and interpret its results)
     - Avoid changing the contents of other directories, as it contains the main emulator/debugger/firmware code provided by the project.
3. Run the CMake build in the top directory. This will generate the debugger, emulator and library.
      ```sh
      mkdir build
      cd build
      cmake ..
      make
      ```
4. Provide a source file that implements [user/microcontroller.h] for that particular microcontroller (initialization, UART, etc), as well as
  any additional files required to implement the firmware side of the debugger.
     - The firmware build (such as a Makefile) needs to be provided by the user. This build will need to compile the files contained in 
       [microcontroller/FILELIST](microcontroller/FILELIST).
