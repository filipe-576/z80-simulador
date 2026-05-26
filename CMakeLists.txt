cmake_minimum_required(VERSION 3.16)
project(z80-simulador LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall -Wextra)
endif()

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")
set(IMGUI_SOURCES
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)

# Simulador z80
add_executable(z80-simulador
    src/vm.cpp
    src/z80/cpu/cpu.cpp
    src/z80/cpu/registers.cpp
    src/z80/memory/memory.cpp
    src/z80/instructions/instruction_set.cpp
    src/z80/gui/gui.cpp
    ${IMGUI_SOURCES}
)

target_include_directories(z80-simulador PRIVATE
    src/z80/cpu
    src/z80/memory
    src/z80/instructions
    src/z80/gui
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${GLFW_INCLUDE_DIRS}
)

target_link_libraries(z80-simulador PRIVATE
    ${GLFW_LDFLAGS}
    OpenGL::GL
)

if(APPLE)
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(COREVIDEO_FRAMEWORK CoreVideo)
    target_link_libraries(z80-simulador PRIVATE
        ${COCOA_FRAMEWORK}
        ${IOKIT_FRAMEWORK}
        ${COREVIDEO_FRAMEWORK}
    )
endif()


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
