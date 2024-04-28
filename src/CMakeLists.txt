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

add_library(fdbg SHARED
		machine.c
		machine.h
		fdbg/libfdbg/connection.cc
		fdbg/libfdbg/connection.hh
		${PROTO_HEADER} ${PROTO_SRC}
)

target_link_libraries(fdbg
		PUBLIC protobuf::libprotobuf
)

protobuf_generate(TARGET fdbg
		LANGUAGE cpp
		PROTOS fdbg/protobuf/request.proto fdbg/protobuf/response.proto
)