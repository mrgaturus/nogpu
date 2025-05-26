if (NOGPU_EXAMPLES)
    add_executable(hello hello.cpp)
    target_link_libraries(hello PRIVATE nogpu)
endif()