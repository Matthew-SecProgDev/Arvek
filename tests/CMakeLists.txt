add_executable(ArvekLoggerTest
    logger_test.cpp
)

target_link_libraries(ArvekLoggerTest
    PRIVATE ArvekEngine
)

add_test(
    NAME ArvekLoggerTest
    COMMAND ArvekLoggerTest
)

add_executable(ArvekAssertTest
    assert_test.cpp
)

target_link_libraries(ArvekAssertTest
    PRIVATE ArvekEngine
)

add_test(
    NAME ArvekAssertPass
    COMMAND ArvekAssertTest
)

add_test(
    NAME ArvekAssertFailure
    CONFIGURATIONS Debug
    COMMAND ${CMAKE_COMMAND}
        "-DTEST_EXECUTABLE=$<TARGET_FILE:ArvekAssertTest>"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/check_assert_failure.cmake"
)

add_executable(ArvekServiceRegistryTest
    service_registry_test.cpp
)

target_link_libraries(ArvekServiceRegistryTest
    PRIVATE ArvekEngine
)

add_test(
    NAME ArvekServiceRegistryTest
    COMMAND ArvekServiceRegistryTest
)