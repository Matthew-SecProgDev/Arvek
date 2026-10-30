cmake_minimum_required(VERSION 3.21)

project(YForge
    VERSION 0.1.0
    DESCRIPTION "A C++ game engine focused on high-performance Vulkan rendering"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Sandbox application for testing engine features.
add_executable(YForgeSandbox
    sandbox/main.cpp
)

# Enable common compiler warnings.
if(MSVC)
    target_compile_options(YForgeSandbox PRIVATE /W4)
else()
    target_compile_options(YForgeSandbox PRIVATE -Wall -Wextra -Wpedantic)
endif()