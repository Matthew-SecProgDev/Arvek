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

add_executable(YForgeAssertTest
    assert_test.cpp
)

target_link_libraries(YForgeAssertTest
    PRIVATE YForgeEngine
)

add_test(
    NAME YForgeAssertPass
    COMMAND YForgeAssertTest
)

add_test(
    NAME YForgeAssertFailure
    CONFIGURATIONS Debug
    COMMAND ${CMAKE_COMMAND}
        "-DTEST_EXECUTABLE=$<TARGET_FILE:YForgeAssertTest>"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/check_assert_failure.cmake"
)

add_executable(YForgeServiceRegistryTest
    service_registry_test.cpp
)

target_link_libraries(YForgeServiceRegistryTest
    PRIVATE YForgeEngine
)

add_test(
    NAME YForgeServiceRegistryTest
    COMMAND YForgeServiceRegistryTest
)