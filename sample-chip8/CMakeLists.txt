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