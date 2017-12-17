# Expected variables:
#     VALGRIND_EXECUTABLE - path the the valgrind.
#     TEST_EXECUTABLE - path to the test binary.

if ("${VALGRIND_EXECUTABLE}" STREQUAL "")
    message (FATAL_ERROR "No valgrind is found!")
endif ()

if (("${TEST_EXECUTABLE}" STREQUAL "") OR (NOT EXISTS ${TEST_EXECUTABLE}))
    message (FATAL_ERROR "Invalid test ${TEST_EXECUTABLE}!")
endif ()

message ("Test: ${TEST_EXECUTABLE}")
set (_cmd ${VALGRIND_EXECUTABLE} --leak-check=yes -q ${TEST_EXECUTABLE})
message ("Command: ${_cmd}")

execute_process(
    COMMAND ${_cmd}
    RESULT_VARIABLE _result
    OUTPUT_VARIABLE _output
    ERROR_VARIABLE _output
)

message("${_output}")

if (NOT "${_result}" STREQUAL "0")
    message (FATAL_ERROR "Test failed with exit code ${_result}")
endif ()

if ("${_output}" MATCHES "[.]*==[0-9]*== [.]*")
    message (FATAL_ERROR "Valgrind reported errors.")
endif ()    


