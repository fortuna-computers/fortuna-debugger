cmake_minimum_required(VERSION 3.27)

project(fortuna-debugger
        VERSION 1.0
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
# external libraries
#
set(NANOPB_SRC_ROOT_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/contrib/nanopb")
set(CMAKE_MODULE_PATH ${NANOPB_SRC_ROOT_FOLDER}/extra)
find_package(Nanopb REQUIRED)
include_directories(
        ${NANOPB_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
)

#
# nanopb messages
#

file(GLOB PROTOS library/protobuf/*.proto)
nanopb_generate_cpp(TARGET proto ${PROTOS})

#
# libfdbg-server
#

set(fdbg-server_src
        library/server/libfdbg-server.c
        library/server/libfdbg-server.h
        library/common/terminal.c
        library/common/terminal.h)

add_library(fdbg-server_obj OBJECT EXCLUDE_FROM_ALL ${fdbg-server_src})
set_property(TARGET fdbg-server_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(fdbg-server_static STATIC $<TARGET_OBJECTS:fdbg-server_obj>)
set_target_properties(fdbg-server_static PROPERTIES OUTPUT_NAME "fgdb-server")
target_link_libraries(fdbg-server_static proto)

add_library(fdbg-server_shared SHARED $<TARGET_OBJECTS:fdbg-server_obj>)
set_target_properties(fdbg-server_shared PROPERTIES OUTPUT_NAME "fgdb-server")
target_link_libraries(fdbg-server_shared proto)

#
# libfdbg-client
#

add_library(fdbg-client STATIC
        library/client/libfdbg-client.c
        library/client/libfdbg-client.h
        library/common/terminal.c
        library/common/terminal.h)
target_link_libraries(fdbg-client proto)

#
# scripts
#

configure_file(scripts/findserial.py . USE_SOURCE_PERMISSIONS COPYONLY)
