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
- [ ] What else can be moved to the library
- [ ] Isolate UI model
- [ ] Refactor UI debugger
- [ ] Lua library (client, server)
- [ ] Move findserial.py code to C++ library

# Compilation

- [x] Install and test chip-8 compiler
- [x] Create interfaces (debugging info, compiler output, ROM, and file list, location in memory)
- [x] Compile
- [ ] Upload to ROM

# CPU

- [ ] Stack in memory window
- [ ] Show PC in memory window

# Video

# Other devices