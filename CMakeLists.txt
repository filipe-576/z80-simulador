cmake_minimum_required(VERSION 3.16)

project(z80-simulador LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall -Wextra)
endif()

add_executable(z80-simulador
    src/main.cpp
    src/cpu/cpu.cpp
    src/cpu/registers.cpp
    src/memory/memory.cpp
    src/instructions/instruction_set.cpp
    src/gui/gui.cpp
)

target_include_directories(z80-simulador PRIVATE
    src/cpu
    src/memory
    src/instructions
    src/gui
)