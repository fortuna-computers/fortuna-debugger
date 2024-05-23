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
  - [x] Clear breakpoints button
- [x] What else is missing?

# Registers

- [x] Registers/flags window

# Memory changes

- [x] Stack in memory window
- [x] Show PC in memory window

# Cycle

- [x] Cycle window

# Run

- [x] Run
  - [x] Start running
    - [x] UI running (restrict buttons, show running indication)
  - [x] Breakpoint hit
  - [x] Scroll to position when breakpoint hit/pause
  - [x] Get run updates
- [x] Pause
- [x] Next

# Recompilation

- [x] Watch source code
- [x] Recompilation support

# Reactory 2

- [x] Check if responses from server are correct (ex. number of registers match)
- [x] Choose theme
- [x] Configuration: debugging level
- [x] Check for message sizes

# Terminal

- [x] Logic for send special commands
  - [x] Protobuf
  - [x] Server logic
  - [x] Client
- [x] Logic for managing special commands from Lua
- [ ] Terminal UI
  - [x] Terminal
  - [ ] ANSI terminal library
- [ ] Rename events to Output events 
- [ ] Reorganize events (actual events instead of IO codes)

# Detour

- [ ] Understand how to send stream messages
- [ ] Read/write RAM stream
- [ ] Computer status registers and flags stream
- [ ] Events stream

# Terminal input

# Video

# Other devices

- [ ] Multiple memories (for SDCard, etc...)

# Refactory 3

- [ ] Cycle in reverse order
- [ ] Logging into a Window (see Demo -> Configuration -> Capture)
- [ ] Improve theme management
- [ ] Lua library (client, server)
- [ ] Increase message size (2 bytes for message size, stream some messages ToDebugger)
- [ ] LuaJIT and emulator in Lua (FFI support?)
- [ ] Unix socket support (for emulator faster speed), TCP socket (for microcontroller wifi support)
- [ ] Windows are keeping visible on restart, even after closed

# CHIP-8 emulator
