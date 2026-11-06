cmake_minimum_required(VERSION 3.21)

project(Arvek
    VERSION 0.1.0
    DESCRIPTION "A C++ game engine focused on high-performance Vulkan rendering"
    LANGUAGES CXX
)

# fmt dependency
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

# GLFW dependency
find_package(glfw3 CONFIG QUIET)

if(NOT TARGET glfw)
    include(FetchContent)

    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
    )

    FetchContent_MakeAvailable(glfw)
endif()

add_subdirectory(engine)
add_subdirectory(sandbox)

include(CTest)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()