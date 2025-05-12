# SPDX-License-Identifier: MIT
# Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
set(NOGPU_OPENGL_SRC
    glad/gl.c
    buffer.cpp
    context.cpp
    framebuffer.cpp
    program.cpp
    texture.cpp
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND NOGPU_OPENGL_SRC driver_linux.cpp)
elseif(WIN32)
    list(APPEND NOGPU_OPENGL_SRC driver_win32.cpp)
endif()

# Define OpenGL Target
add_definitions(-DNOGPU_OPENGL)
add_library(nogpu_opengl OBJECT
    ${NOGPU_OPENGL_SRC})
