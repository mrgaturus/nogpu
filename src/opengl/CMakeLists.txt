# SPDX-License-Identifier: MIT
# Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
add_definitions(-DNOGPU_OPENGL)
add_library(nogpu_opengl OBJECT
    buffer.cpp
    context.cpp
    driver.cpp)
