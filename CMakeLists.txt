# SPDX-License-Identifier: MIT
# Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
cmake_minimum_required(VERSION 3.16)
project(nogpu
    VERSION 0.1.0
    DESCRIPTION "simple gpu api abstraction library"
    LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
option(NOGPU_DEBUG "enable driver debugging" OFF)
option(NOGPU_EXAMPLES "compile examples" OFF)
option(NOGPU_GLFW "use GLFW windowing" OFF)
option(NOGPU_SDL3 "use SDL3 windowing" OFF)
option(NOGPU_SDL2 "use SDL2 windowing" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_library(nogpu STATIC
    src/map_crc32.cpp
    src/map.cpp
    src/nogpu.cpp
    src/private.cpp
    src/report.cpp)

# Windowing Surface: GLFW 3.4
if (NOGPU_GLFW)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    find_package(glfw3 3.4 REQUIRED)
    add_definitions(-DNOGPU_GLFW)
    target_link_libraries(nogpu PRIVATE glfw)
endif()

# Windowing Surface: SDL
if (NOGPU_SDL3)
    find_package(SDL3 REQUIRED)
    add_definitions(-DNOGPU_SDL3)
    target_link_libraries(nogpu PRIVATE SDL3::SDL3)
elseif (NOGPU_SDL2)
    find_package(SDL2 REQUIRED)
    add_definitions(-DNOGPU_SDL2)
    target_link_libraries(nogpu PRIVATE SDL2::SDL2)
endif()

include_directories(include)
add_subdirectory(src/opengl)
add_subdirectory(src/vulkan)
# Compile nogpu library
target_include_directories(nogpu PRIVATE src)
target_include_directories(nogpu_opengl PRIVATE src)
target_include_directories(nogpu_vulkan PRIVATE src)
target_link_libraries(nogpu PRIVATE nogpu_opengl)
target_link_libraries(nogpu PRIVATE nogpu_vulkan)
# Compile nogpu examples
add_subdirectory(examples)
