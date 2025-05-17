if (NOGPU_EXAMPLES)
    add_executable(triangle triangle.cpp)
    target_link_libraries(triangle PRIVATE nogpu)
endif()