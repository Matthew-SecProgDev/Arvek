if(NOT DEFINED TEST_EXECUTABLE)
    message(FATAL_ERROR "TEST_EXECUTABLE is required")
endif()

execute_process(
    COMMAND "${TEST_EXECUTABLE}" fail
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)

if("${result}" STREQUAL "0")
    message(FATAL_ERROR "Assertion did not terminate the process")
endif()

if(NOT error MATCHES "ASSERTION FAILED")
    message(FATAL_ERROR "Expected assertion diagnostic was not found:\n${error}")
endif()