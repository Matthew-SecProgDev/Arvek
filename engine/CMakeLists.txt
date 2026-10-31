add_library(ArvekEngine STATIC
    src/core/engine.cpp
    src/core/logger.cpp
    src/core/assert.cpp
    src/core/service_registry.cpp
)

add_library(Arvek::Engine ALIAS ArvekEngine)

target_compile_features(ArvekEngine PUBLIC cxx_std_17)

target_include_directories(ArvekEngine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(ArvekEngine
    PUBLIC
        fmt::fmt
)

if(MSVC)
    target_compile_options(ArvekEngine PRIVATE /W4 /utf-8)
else()
    target_compile_options(ArvekEngine PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()