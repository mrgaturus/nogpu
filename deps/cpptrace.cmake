if (NOGPU_DEBUG)
    # Avoid Compile Static in Linux
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(CPPTRACE_USE_EXTERNAL_ZSTD ON)
        set(CPPTRACE_USE_EXTERNAL_LIBDWARF ON)
        set(CPPTRACE_FIND_LIBDWARF_WITH_PKGCONFIG ON)
    endif()

    include(FetchContent)
    FetchContent_Declare(
        cpptrace
        GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
        GIT_TAG        v0.8.3 # <HASH or TAG>
    )
    FetchContent_MakeAvailable(cpptrace)
    target_link_libraries(nogpu PRIVATE cpptrace::cpptrace)
    add_definitions(-DNOGPU_DEBUG)
    add_definitions(-g)

    # cpptrace.dll in win32
    if(WIN32)
        add_custom_command(
            TARGET nogpu POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:cpptrace::cpptrace>
            $<TARGET_FILE_DIR:nogpu>
        )
    endif()
endif()
