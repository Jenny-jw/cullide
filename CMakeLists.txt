# =============================================================================
# CMake is a build tool. You do not compile with `g++ main.cpp` by hand.
# You describe the project here; CMake generates Makefiles (or Ninja files),
# then those actually compile.
#
# Two commands you will use:
#   cmake -S . -B build          # configure: read this file, download deps
#   cmake --build build          # compile
# =============================================================================

# CMake 3.14+ has FetchContent_MakeAvailable (the easy download API).
# This machine has 3.22, so 3.16 is a safe minimum.
cmake_minimum_required(VERSION 3.16)

# Project name. This becomes the Visual Studio / Makefile project name.
# LANGUAGES tells CMake we compile C (glad) and C++ (everything else).
project(CULLIDE LANGUAGES C CXX)

# Use C++17. REQUIRED means configure fails if the compiler is too old.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Put compile_commands.json in the build dir (Cursor / clangd can use it).
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Debug if the user did not pass -DCMAKE_BUILD_TYPE=Release.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# -----------------------------------------------------------------------------
# FetchContent: CMake clones a git repo at configure time, then builds it
# as part of THIS project. First configure needs the internet; later ones reuse
# the copy under build/_deps/.
# -----------------------------------------------------------------------------
include(FetchContent)

# --- GLFW: window + input + OpenGL context -----------------------------------
# These options must be set BEFORE MakeAvailable, so GLFW's own CMakeLists
# sees them. CACHE BOOL "" FORCE writes into CMake's cache.
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
# GLFW 3.4 on Linux enables Wayland + X11. Wayland needs wayland-scanner;
# WSL uses X11 (WSLg). Turn Wayland off so we only need the X11 -dev packages.
set(GLFW_BUILD_WAYLAND  OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4          # pin a version so builds stay reproducible
    GIT_SHALLOW    TRUE         # clone only that tag, not the whole history
)

# --- GLM: header-only math (vec3, mat4). No .cpp to compile. -----------------
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
    GIT_SHALLOW    TRUE
)

# --- Assimp: loads .obj / .mtl / textures metadata ---------------------------
# Assimp is large. Turn off tests and tools so it compiles faster.
set(ASSIMP_BUILD_TESTS         OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS  OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL             OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS  OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG        v5.4.3
    GIT_SHALLOW    TRUE
)

# --- GLAD: loads OpenGL function pointers (glDrawArrays, glEnable, ...) ------
# Your code includes "glad/gl.h" and calls gladLoaderLoadGL() — that is GLAD 2.

# Clone (if needed) and add each repo to this build.
FetchContent_MakeAvailable(glfw glm assimp)

# OpenGL itself comes from the OS / GPU driver. You cannot git-clone it.
find_package(OpenGL REQUIRED)

# -----------------------------------------------------------------------------
# Our program
# -----------------------------------------------------------------------------
add_executable(GL
    main.cpp
    glad/gl.c
)

# Headers in this folder: shader.hpp, camera.hpp, model.hpp, box.hpp, ...
# PRIVATE = only this target needs these include paths.
target_include_directories(GL PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link order does not usually matter with CMake imported targets.
# PRIVATE = GL uses these libs; nothing else links against GL.
target_link_libraries(GL PRIVATE
    glfw
    glm::glm
    assimp
    OpenGL::GL
)

# Warnings for our code only (not for GLFW/Assimp, which we do not own).
if(MSVC)
    target_compile_options(GL PRIVATE /W4)
else()
    target_compile_options(GL PRIVATE -Wall -Wextra)
endif()
