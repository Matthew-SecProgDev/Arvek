add_library(YForgeEngine STATIC
    src/core/engine.cpp
    src/core/logger.cpp
)

add_library(YForge::Engine ALIAS YForgeEngine)

target_compile_features(YForgeEngine PUBLIC cxx_std_17)

target_include_directories(YForgeEngine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(YForgeEngine
    PUBLIC
        fmt::fmt
)

if(MSVC)
    target_compile_options(YForgeEngine PRIVATE /W4 /utf-8)
else()
    target_compile_options(YForgeEngine PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()