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
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/library/client
        ${CMAKE_CURRENT_SOURCE_DIR}/library/server
)

file(GLOB PROTOS library/protobuf/*.proto)

#
# external libraries
#

set(NANOPB_SRC_ROOT_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/contrib/nanopb")
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

nanopb_generate_cpp(TARGET proto ${PROTOS})

set(fdbg-server_src
        library/server/libfdbg-server.c
        library/server/libfdbg-server.h
        library/common/terminal.c
        library/common/terminal.h
        library/common/common.h)

set_source_files_properties(library/server/libfdbg-server.h OBJECT_DEPENDS proto)
set_source_files_properties(library/server/libfdbg-server.c OBJECT_DEPENDS proto)

add_library(fdbg-server_obj OBJECT EXCLUDE_FROM_ALL ${fdbg-server_src})
set_property(TARGET fdbg-server_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(fdbg-server_static STATIC $<TARGET_OBJECTS:fdbg-server_obj>)
set_target_properties(fdbg-server_static PROPERTIES OUTPUT_NAME "fgdb-server")
target_link_libraries(fdbg-server_static PUBLIC proto)
add_dependencies(fdbg-server_static proto)

add_library(fdbg-server_shared SHARED $<TARGET_OBJECTS:fdbg-server_obj>)
set_target_properties(fdbg-server_shared PROPERTIES OUTPUT_NAME "fgdb-server")
target_link_libraries(fdbg-server_shared PUBLIC proto)
add_dependencies(fdbg-server_static proto)

#
# libfdbg-client (C++)
#

add_library(fdbg-client STATIC
        library/common/common.h
        library/client/libfdbg-client.cc
        library/client/libfdbg-client.hh)

target_link_libraries(fdbg-client PUBLIC ${Protobuf_LIBRARIES})

protobuf_generate(TARGET fdbg-client
        LANGUAGE cpp
        PROTOS ${PROTOS}
)

#
# library unit tests
#

add_executable(test-library library/test/test-library.cc)
target_link_libraries(test-library PUBLIC fdbg-client fdbg-server_static protobuf::libprotobuf-lite absl::log_internal_check_op)
add_test(name "test library"
         COMMAND test-library)

#
# debugger
#

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(contrib/glfw)

set(IMGUI_SRC
        contrib/imgui/imgui.cpp
        contrib/imgui/imgui_demo.cpp
        contrib/imgui/imgui_draw.cpp
        contrib/imgui/imgui_tables.cpp
        contrib/imgui/imgui_widgets.cpp
        contrib/imgui/backends/imgui_impl_glfw.cpp
        contrib/imgui/backends/imgui_impl_opengl3.cpp
        contrib/glad/src/gl.c
)

add_executable(f-debugger
        ${IMGUI_SRC}
        debugger/main.cc
        debugger/ui.cc
        debugger/ui.hh
        debugger/windows/window.hh
        debugger/windows/demo.cc
        debugger/windows/demo.hh
        debugger/windows/messagebox.cc
        debugger/windows/messagebox.hh
        debugger/windows/startup.cc
        debugger/windows/startup.hh
        debugger/config.cc
        debugger/config.hh
        debugger/emulator/emulator.cc
        debugger/emulator/emulator.hh
        debugger/exceptions/exceptions.hh
)

target_include_directories(f-debugger PUBLIC
        contrib/glad/include
        contrib/imgui
        contrib/imgui/backends
        ${Protobuf_INCLUDE_DIRS}
)

target_link_libraries(f-debugger PUBLIC fdbg-client fdbg-server_static glfw protobuf::libprotobuf-lite absl::log_internal_check_op)

#
# scripts
#

configure_file(scripts/findserial.py . USE_SOURCE_PERMISSIONS COPYONLY)