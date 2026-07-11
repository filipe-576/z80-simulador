cmake_minimum_required(VERSION 3.16)
project(z80-simulador LANGUAGES CXX)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

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
    
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
    external/imgui/imgui_demo.cpp
)
target_link_libraries(z80-simulador PRIVATE
    glfw
    OpenGL::GL
)

target_include_directories(z80-simulador PRIVATE
    src/z80/cpu
    src/z80/memory
    src/z80/instructions
    src/z80/gui
)

# Processador de Macros
add_executable(macro_processor
    src/macro/main.cpp
    src/macro/macro_processor.cpp
)

target_include_directories(macro_processor PRIVATE
    src/macro
)

# Montador
add_executable(assembler
    src/assembler/main.cpp
    src/assembler/assembler.cpp
)

target_include_directories(assembler PRIVATE
    src/assembler
)

# Ligador
add_executable(linker
    src/linker/main.cpp
    src/linker/linker.cpp
)

target_include_directories(linker PRIVATE
    src/linker
)

target_include_directories(z80-simulador PRIVATE
    external/imgui
    external/imgui/backends
    
)