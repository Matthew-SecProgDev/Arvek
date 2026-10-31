cmake_minimum_required(VERSION 3.21)

project(Arvek
    VERSION 0.1.0
    DESCRIPTION "A C++ game engine focused on high-performance Vulkan rendering"
    LANGUAGES CXX
)

find_package(fmt CONFIG QUIET)

if(NOT fmt_FOUND)
    include(FetchContent)

    FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG e69e5f977d458f2650bb346dadf2ad30c5320281
    )

    FetchContent_MakeAvailable(fmt)
endif()

add_subdirectory(engine)
add_subdirectory(sandbox)

include(CTest)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()