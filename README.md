# fortuna-debugger

Fortuna Debugger is a set of tools for computer designers that, toghether, provide debugging capability to any custom computer.

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

1. Fork the repository, or add as a submodule in another project. Remove the directory `sample-chip8` (or use it as a sample).
2. Update the following files:
     - `src/machine.h`: machine characteristics
     - `src/emulation.cc`: emulator code
     - `src/compiler.cc`: compilation code (call the compiler and interpret its results)
     - Avoid changing the contents of directory `src/fdbg/`, as it contains the main emulator/debugger/firmware code provided by the project.
3. Run the CMake build in the top directory. This will generate the debugger, emulator and library.
      ```sh
      mkdir build
      cd build
      cmake ..
      make
      ```
4. Update the `src/microcontroller.c` with the microcontroller specific code (such as initialization, UART, etc).
     - The firmware build (such as a Makefile) needs to be provided by the user. This build will need to compile the files in `src/microcontroller.c` and `src/fdbg/firmware/*`, and upload it to the microcontroller.
