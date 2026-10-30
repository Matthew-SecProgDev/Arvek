add_executable(YForgeLoggerTest
    logger_test.cpp
)

target_link_libraries(YForgeLoggerTest
    PRIVATE YForgeEngine
)

add_test(
    NAME YForgeLoggerTest
    COMMAND YForgeLoggerTest
)