cmake_minimum_required(VERSION 3.21)

project(YForge
    VERSION 0.1.0
    DESCRIPTION "A C++ game engine focused on high-performance Vulkan rendering"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Engine Library
add_library(YForgeEngine STATIC
    engine/src/Engine.cpp
)

target_compile_features(YForgeEngine PUBLIC cxx_std_17)

target_include_directories(YForgeEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/include
)

# Sandbox application for testing engine features.
add_executable(YForgeSandbox
    sandbox/main.cpp
)

target_link_libraries(YForgeSandbox PRIVATE
    YForgeEngine
)

# Compiler Warnings
function(yforge_set_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
        )
    endif()
endfunction()

yforge_set_warnings(YForgeEngine)
yforge_set_warnings(YForgeSandbox)