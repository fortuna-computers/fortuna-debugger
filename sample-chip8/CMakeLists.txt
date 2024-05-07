cmake_minimum_required(VERSION 3.27)

set(PROJECT_NAME simple-chip8)

project(${PROJECT_NAME} VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} SHARED
        emulator.c
        machine.c
        compiler.c
)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

# uncomment this section to add lua to the build

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
add_subdirectory(lua)

target_include_directories(${PROJECT_NAME} PUBLIC lua::header)
target_link_libraries(${PROJECT_NAME} PRIVATE lua::lib)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/compiler_lua.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND sh ARGS -C "xxd -i compiler.lua > ${CMAKE_CURRENT_BINARY_DIR}/compiler_lua.h")

set_property(SOURCE compiler.c APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/compiler_lua.h)