# SPDX-License-Identifier: MIT
# Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
add_definitions(-DNOGPU_OPENGL)
add_library(nogpu_opengl OBJECT
    private/glad.c
    buffer_vao.cpp
    buffer.cpp
    commands.cpp
    context.cpp
    driver.cpp
    framebuffer_target.cpp
    framebuffer_texture.cpp
    framebuffer.cpp
    pipeline.cpp
    shader.cpp
    texture_standard/texture_1d.cpp
    texture_standard/texture_2d.cpp
    texture_standard/texture_3d.cpp
    texture_standard/texture_cubemap.cpp
    texture_standard/texture_cubemap_array.cpp
    texture_compressed/texture_1d.cpp
    texture_compressed/texture_2d.cpp
    texture_compressed/texture_3d.cpp
    texture_compressed/texture_cubemap.cpp
    texture_compressed/texture_cubemap_array.cpp
    texture_buffer.cpp
    texture_compat.cpp
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
