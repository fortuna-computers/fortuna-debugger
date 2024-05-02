cmake_minimum_required(VERSION 3.27)

project(
	fortuna-debugger
	VERSION 1.0
	LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(
		${Protobuf_INCLUDE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}
		fdbg/libfdbg
		fdbg/contrib/glad/include
		fdbg/contrib/glfw/include
		fdbg/contrib/imgui
		fdbg/contrib/imgui/backends
)


#
# Clone submodules
#
add_custom_target(submodule ALL
		DEPENDS submodule)
add_custom_command(OUTPUT submodule
		PRE_BUILD
		COMMAND test -e ${CMAKE_SOURCE_DIR}/src/fdbg/contrib/imgui/imgui.h || git submodule update --init --remote --recursive
		COMMENT "Checking if submodule update is required...")

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
# debugger
#

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(fdbg/contrib/glfw)

set(IMGUI_SRC
		fdbg/contrib/imgui/imgui.cpp
		fdbg/contrib/imgui/imgui_demo.cpp
		fdbg/contrib/imgui/imgui_draw.cpp
		fdbg/contrib/imgui/imgui_tables.cpp
		fdbg/contrib/imgui/imgui_widgets.cpp
		fdbg/contrib/imgui/backends/imgui_impl_glfw.cpp
		fdbg/contrib/imgui/backends/imgui_impl_opengl3.cpp
		fdbg/contrib/glad/src/gl.c
)

add_executable(f-debugger
		${IMGUI_SRC}
		fdbg/debugger/main.cc
		fdbg/debugger/ui.cc
		fdbg/debugger/ui.hh
		fdbg/debugger/windows/window.hh
		fdbg/debugger/windows/demo.cc
		fdbg/debugger/windows/demo.hh
		fdbg/debugger/windows/messagebox.cc
		fdbg/debugger/windows/messagebox.hh
		fdbg/debugger/windows/startup.cc
		fdbg/debugger/windows/startup.hh
		fdbg/debugger/config.cc
		fdbg/debugger/config.hh
		fdbg/debugger/emulator/emulator.cc
		fdbg/debugger/emulator/emulator.hh
)

target_include_directories(f-debugger PUBLIC fdbg/libfdbg)

target_link_libraries(f-debugger fdbg glfw)
