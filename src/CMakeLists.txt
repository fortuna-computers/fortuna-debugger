cmake_minimum_required(VERSION 3.27)

project(
	fortuna-debugger
	VERSION 1.0
	LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf CONFIG REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${Protobuf_INCLUDE_DIR})

#
# libfdbg.so
#

add_library(fdbg SHARED
		${PROTO_HEADER} ${PROTO_SRC}
		machine.c
		machine.h
		fdbg/libfdbg/fdbg.hh
		fdbg/libfdbg/client.cc
		fdbg/libfdbg/client.hh
		fdbg/libfdbg/client.cc
		fdbg/libfdbg/client.hh
		fdbg/libfdbg/exceptions.hh
		fdbg/libfdbg/server.cc
		fdbg/libfdbg/server.hh
		fdbg/libfdbg/serial.cc
		fdbg/libfdbg/serial.hh
		fdbg/libfdbg/serial.inl
)

target_link_libraries(fdbg
		PUBLIC protobuf::libprotobuf
)

protobuf_generate(TARGET fdbg
		LANGUAGE cpp
		PROTOS fdbg/protobuf/to-computer.proto fdbg/protobuf/to-debugger.proto
)

#
# emulator
#

add_executable(f-emulator
		fdbg/emulator/main.cc
		fdbg/emulator/emulator_cb.h
		emulation.c
		emulation.h
)

target_include_directories(f-emulator PUBLIC fdbg/libfdbg)

target_link_libraries(f-emulator fdbg)

#
# debugger
#

add_executable(f-debugger
		fdbg/debugger/main.cc
)

target_include_directories(f-debugger PUBLIC fdbg/libfdbg)

target_link_libraries(f-debugger fdbg)
