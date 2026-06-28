cmake_minimum_required(VERSION 3.16)
project(z80-simulador LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall -Wextra)
endif()



# Simulador z80
add_executable(z80-simulador
src/vm.cpp
src/z80/cpu/cpu.cpp
src/z80/cpu/registers.cpp
src/z80/memory/memory.cpp
src/z80/instructions/instruction_set.cpp
src/z80/gui/gui.cpp
)

target_include_directories(z80-simulador PRIVATE
src/z80/cpu
src/z80/memory
src/z80/instructions
src/z80/gui
)
target_compile_definitions(z80-simulador PRIVATE PROJ_DIR="${CMAKE_SOURCE_DIR}")

# Processador de Macros
add_executable(macro_processor
src/macro/main.cpp
src/macro/macro_processor.cpp
src/utils.cpp
)

target_include_directories(macro_processor PRIVATE
src/macro
)
target_compile_definitions(macro_processor PRIVATE PROJ_DIR="${CMAKE_SOURCE_DIR}")

# Montador
add_executable(assembler
src/assembler/main.cpp
src/assembler/assembler.cpp
src/utils.cpp
)

target_include_directories(assembler PRIVATE
src/assembler
)
target_compile_definitions(assembler PRIVATE PROJ_DIR="${CMAKE_SOURCE_DIR}")

# Ligador
add_executable(linker
src/linker/main.cpp
src/linker/linker.cpp
src/utils.cpp
)

target_include_directories(linker PRIVATE
src/linker
)
target_compile_definitions(linker PRIVATE PROJ_DIR="${CMAKE_SOURCE_DIR}")