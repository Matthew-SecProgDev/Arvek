add_executable(ArvekSandbox
    main.cpp
)

target_link_libraries(ArvekSandbox
    PRIVATE ArvekEngine
)

if(MSVC)
    target_compile_options(ArvekSandbox PRIVATE /W4 /utf-8)
else()
    target_compile_options(ArvekSandbox PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()