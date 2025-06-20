# SPDX-License-Identifier: MIT
# Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
add_definitions(-DNOGPU_OPENGL)
add_library(nogpu_opengl OBJECT
    glad/glad.c
    buffer_vao.cpp
    buffer.cpp
    commands.cpp
    context.cpp
    framebuffer.cpp
    shader.cpp
    texture/texture_1d.cpp
    texture/texture_2d.cpp
    texture/texture_buffer.cpp
    texture_values.cpp
    texture.cpp
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(PkgConfig REQUIRED)
    pkg_search_module(EGL REQUIRED egl)
    # libglvnd libegl.so dependency
    target_include_directories(nogpu_opengl PRIVATE ${EGL_INCLUDE_DIRS})
    target_link_libraries(nogpu_opengl PRIVATE ${EGL_LIBRARIES})
    target_sources(nogpu_opengl PRIVATE driver_linux.cpp)    
elseif(WIN32)
    target_sources(nogpu_opengl PRIVATE driver_win32.cpp)
endif()
