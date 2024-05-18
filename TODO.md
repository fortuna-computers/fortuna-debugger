# Basic setup

- [x] Basic protobuf setup
  - [x] Ack
- [x] Client/server setup
- [x] Create initial emulator
  - [x] Options
  - [x] Ack

# Initial debugger

- [x] OpenGL and UI
- [x] Initial startup screen (emulator or serial)
  - [x] Run emulator (as thread)
- [x] Initial ack
  - [x] Treat errors
- [x] Connect to real hardware
  - [x] Auto-detect serial port
  - [x] Start real hardware CHIP-8 project

# RAM

- [x] Memory commands in proto
- [x] Memory commands in library (client/server)
- [x] Memory in emulator
- [x] Memory in debugger
 - [x] Create debugger model
- [x] Memory in computer (CHIP-8)

# Communication improvements

- [x] Ready signal
- [x] Treat communication errors

# DLL

- [x] Can the user emulator be a DLL?

# Debugger

- [x] Main menu
- [x] Config window
- [x] Demo window

# Refactory

- [x] Move machine id to Lua and library
- [x] Move compilation to library
- [x] Implement Compile & Upload
- [x] Run emulator independently
- [x] What else can be moved to the library
- [x] Start emulator from client
  - [x] Serial port synchronization
- [x] Refactor UI debugger
  - [x] Review model
  - [x] Add emulator path
  - [x] Create own ini management
  - [x] Fix error when loading machine multiple times

# Compilation

- [x] Install and test chip-8 compiler
- [x] Create interfaces (debugging info, compiler output, ROM, and file list, location in memory)
- [x] Compile
- [x] Upload to ROM

# Bugfixes

- [x] Memory is not uploading
- [x] Ordering issue when building in parallel
- [x] ./findserial.py path

# Code

- [x] Protobuf messages
  - [x] Registers in machine definition
  - [x] Library request/response
- [x] Show code in window
  - [x] Color PC
- [x] Syntax highlighting
- [x] Step (simple, advanced)
  - [x] Protobuf
  - [x] Library
  - [x] UI
    - [x] Move selected line
- [x] Breakpoints
  - [x] Protobuf
  - [x] Library
  - [x] UI
  - [ ] Clear breakpoints button
- [ ] What else is missing?

# Registers

- [x] Registers/flags window

# Cycle

- [ ] Cycle window

# Memory changes

- [ ] Stack in memory window
- [ ] Show PC in memory window

# Run

- [ ] Run
- [ ] Pause
- [ ] Next

# Recompilation

- [ ] Watch source code
- [ ] Recompilation support

# Reactory 2

- [ ] Lua library (client, server)
- [ ] Configuration: debugging level
- [ ] Move findserial.py code to C++ library
- [ ] Move emulator to Lua (?) and possible call C code from it
  - [ ] LuaJIT

# Terminal

# Video

# Other devices