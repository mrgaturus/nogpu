# SPDX-License-Identifier: MIT
# Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
add_definitions(-DNOGPU_VULKAN)
add_library(nogpu_vulkan OBJECT
    buffer.cpp
    context.cpp
    driver.cpp)
