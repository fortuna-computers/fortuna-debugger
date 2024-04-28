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
* A library (`libfdbg.so`) that allows the user to write its own programs to run onboard (like test suites, for example).

A sample implementation is provided for running an emulator version of the [CHIP-8](https://chip-8.github.io/) on a Raspberry Pi Pico.

## Implementing a new architecture

1. Fork the repository, or add as a submodule in another project.
2. Implement all functions descibed in the headers in directory `src/`.
  - Avoid changing the contents of directory `src/fdbg/`, as it contains the main emulator/debugger/firmware code provided by the project.
  - Edit `src/debugger.mk` to add anything to the build process.
3. Run `make` in the top directory. This will generate the debugger, emulator and library.
4. Implement `src/microcontroller.mk`. This will generate and upload the debugging firmware to the microcontroller. Use `make upload` to upload the firmware.
