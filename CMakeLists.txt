cmake_minimum_required(VERSION 3.27)

project(fortuna-debugger
        VERSION 1.0
        LANGUAGES C CXX)

#
# configuration
#

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/library/client
        ${CMAKE_CURRENT_SOURCE_DIR}/src/library/server
)

file(GLOB PROTOS src/library/protobuf/*.proto)

#
# external libraries
#

set(NANOPB_SRC_ROOT_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/src/contrib/nanopb")
set(CMAKE_MODULE_PATH ${NANOPB_SRC_ROOT_FOLDER}/extra)

find_package(Nanopb REQUIRED)
find_package(Protobuf REQUIRED)
find_package(absl REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(
        ${Protobuf_INCLUDE_DIRS}
        ${NANOPB_INCLUDE_DIRS}
)

#
# libfdbg-server (C)
#

nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTOS})

set(fdbg-server_src
        src/library/server/libfdbg-server.c
        src/library/server/libfdbg-server.h
        src/library/common/terminal.c
        src/library/common/terminal.h
        src/library/common/common.h
        ${PROTO_SRCS}
        ${PROTO_HDRS}
)

add_library(fdbg-server_obj OBJECT EXCLUDE_FROM_ALL ${fdbg-server_src})
set_property(TARGET fdbg-server_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(fdbg-server_static STATIC $<TARGET_OBJECTS:fdbg-server_obj>)
set_target_properties(fdbg-server_static PROPERTIES OUTPUT_NAME "fgdb-server")

add_library(fdbg-server_shared SHARED $<TARGET_OBJECTS:fdbg-server_obj>)
set_target_properties(fdbg-server_shared PROPERTIES OUTPUT_NAME "fgdb-server")

#
# libfdbg-client (C++)
#

add_library(fdbg-client STATIC
        src/library/common/common.h
        src/library/common/terminal.h
        src/library/common/terminal.c
        src/library/client/libfdbg-client.cc
        src/library/client/libfdbg-client.hh)

target_link_libraries(fdbg-client PUBLIC ${Protobuf_LIBRARIES})

protobuf_generate(TARGET fdbg-client
        LANGUAGE cpp
        PROTOS ${PROTOS}
)

#
# library unit tests
#

add_executable(test-library src/library/test/test-library.cc)
target_link_libraries(test-library PUBLIC fdbg-client fdbg-server_static protobuf::libprotobuf-lite absl::log_internal_check_op)
add_test(name "test library"
         COMMAND test-library)

add_executable(test-microcontroller src/library/test/test-microcontroller.cc)
target_link_libraries(test-microcontroller PUBLIC fdbg-client protobuf::libprotobuf-lite absl::log_internal_check_op)
add_test(name "test microcontroller"
        COMMAND test-microcontroller)

#
# debugger
#

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(src/contrib/glfw)

set(IMGUI_SRC
        src/contrib/imgui/imgui.cpp
        src/contrib/imgui/imgui_demo.cpp
        src/contrib/imgui/imgui_draw.cpp
        src/contrib/imgui/imgui_tables.cpp
        src/contrib/imgui/imgui_widgets.cpp
        src/contrib/imgui/backends/imgui_impl_glfw.cpp
        src/contrib/imgui/backends/imgui_impl_opengl3.cpp
        src/contrib/glad/src/gl.c
)

add_executable(f-debugger
        ${IMGUI_SRC}
        src/debugger/model.hh
        src/debugger/model.cc
        src/debugger/main.cc
        src/debugger/ui.cc
        src/debugger/ui.hh
        src/debugger/windows/window.hh
        src/debugger/windows/demo.cc
        src/debugger/windows/demo.hh
        src/debugger/windows/messagebox.cc
        src/debugger/windows/messagebox.hh
        src/debugger/windows/startup.cc
        src/debugger/windows/startup.hh
        src/debugger/config.cc
        src/debugger/config.hh
        src/debugger/emulator/emulator.cc
        src/debugger/emulator/emulator.hh
        src/debugger/exceptions/exceptions.hh
        src/debugger/windows/memory.cc
        src/debugger/windows/memory.hh
        src/debugger/load.cc
        src/debugger/load.hh
)

target_include_directories(f-debugger PUBLIC
        src/contrib/glad/include
        src/contrib/imgui
        src/contrib/imgui/backends
        ${Protobuf_INCLUDE_DIRS}
)

target_link_libraries(f-debugger PUBLIC fdbg-client fdbg-server_static glfw protobuf::libprotobuf-lite absl::log_internal_check_op)

#
# sample project (CHIP-8)
#

add_subdirectory(sample-chip8)

#
# scripts
#

configure_file(src/scripts/findserial.py . USE_SOURCE_PERMISSIONS COPYONLY)