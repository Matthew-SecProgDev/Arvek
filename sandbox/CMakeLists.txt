add_executable(YForgeSandbox
    main.cpp
)

target_link_libraries(YForgeSandbox
    PRIVATE YForgeEngine
)

if(MSVC)
    target_compile_options(YForgeSandbox PRIVATE /W4 /utf-8)
else()
    target_compile_options(YForgeSandbox PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()