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

# Protobuf modifications

- [x] Remove malloc from server
- [x] Autogenerate max from options file (?)
- [x] Increase message size (2 bytes or protobuf format)
  - [x] Increase size of the arrays (max 2 kB)
- [ ] Reduce int sizes

# Terminal

- [x] Terminal size from Lua
- [x] Protobuf commands:
  - [x] Terminal RX/TX proto
  - [x] Client/server
- [x] Terminal UI
  - [x] Terminal
    - [x] Choose raw/ANSI
    - [x] New line (CR, LF, CR+LF)
  - [x] Clear button
  - [x] Write to the terminal
    - [x] Raw
    - [x] ANSI
  - [x] Save terminal config
- [x] Keyboard input
  - [x] Mid-step
  - [ ] While running

# Other devices

- [x] Multiple memories (for SDCard, etc...)
  - [x] `memories` in Lua/machine
  - [x] Protobuf / library
  - [x] Multiple memories in debugger

# Refactory 3

- [x] Config
  - [x] Windows are keeping visible on restart, even after closed
- [x] UI
  - [x] Cycle in reverse order
- [x] Lua library (client, server)
- [x] Documentation (sample emulator)
- [x] Server function to push character
- [x] Push character when running
- [x] Key capture: fix issue with SHIFT
- [x] Report error on incorrect instruction (or other error)
- [x] Versioning, packaging
  - [x] Build release by default
  - [x] Fix tests
  - [x] make install
  - [x] check for dependencies
  - [x] compile on Linux
- [x] Documentation

# Refactory 4

- [ ] Redo build system
  - [x] Reorganize dependencies
  - [x] Fetch git submodules
  - [x] Test on linux
  - [x] Fix warnings
  - [x] Debugging build?
  - [x] Autodetect
  - [x] Sample emulator?
  - [x] Adjust README
- [ ] Fix tests

# Refactor 5

- [ ] Separate library in a different repo
- [ ] Fix tabs in the editor
- [ ] Backspace not deleting previous character
- [ ] Restart debugger
- [ ] Add breakpoint while running
- [ ] Save if last execution was emulator or real hardware
- [ ] Better syntax highlighting (?)
- [ ] Documentation

# Video

# Someday, likely never

- [ ] LuaJIT and emulator in Lua (FFI support?)
- [ ] Unix socket support (for emulator faster speed), TCP socket (for microcontroller wifi support)
- [ ] Improve config class to manage variables as if they were C++ variables
- [ ] Logging into a Window (see Demo -> Configuration -> Capture)
