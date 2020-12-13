# In order to run the unittests the following conditions must be true:
#   - find_package (CxxTest) was exectued, CXXTEST_FOUND is defined and has true value.

if (NOT BUILD_TESTING)
    return ()
endif ()    

set (COMPONENT_NAME "comms")

#################################################################

function (test_func test_suite_name)
    set (tests "${CMAKE_CURRENT_SOURCE_DIR}/${test_suite_name}.th")

    set (name "${COMPONENT_NAME}.${test_suite_name}Test")

    set (valgrand_args)
    if (VALGRIND_EXECUTABLE)
        set (valgrand_args VALGRIND_EXECUTABLE ${VALGRIND_EXECUTABLE})
    endif ()    

    cc_cxxtest_add_test (NAME ${name} SRC ${tests} ${valgrand_args})

    target_compile_options(${name} PRIVATE
        $<$<CXX_COMPILER_ID:GNU>: ${CC_SANITIZER_OPTS}>
        $<$<CXX_COMPILER_ID:Clang>:${CC_SANITIZER_OPTS}>
    ) 

    if (NOT "${CC_SANITIZER_OPTS}" STREQUAL "")
        target_link_options(${name} PRIVATE
            $<$<CXX_COMPILER_ID:GNU>:${CC_SANITIZER_OPTS}>
            $<$<CXX_COMPILER_ID:Clang>:${CC_SANITIZER_OPTS}>
        ) 
    endif ()

endfunction ()

#################################################################

set (CC_SANITIZER_OPTS)
if (CC_UNIT_TESTS_USE_SANITIZERS AND
    ((CMAKE_COMPILER_IS_GNUCC) OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")))
    set (CC_SANITIZER_OPTS 
        -fno-omit-frame-pointer 
        -fno-sanitize-recover=address 
        -fsanitize=address
        -fno-sanitize-recover=undefined
        -fsanitize=undefined)
endif ()

if ((NOT "${CC_SANITIZER_OPTS}" STREQUAL "") AND (${CMAKE_VERSION} VERSION_LESS "3.13"))
    message (WARNING "The CMake version is too old, expected at least 3.13, sanitizers are disabled")
    set (CC_SANITIZER_OPTS)
endif ()

if ((NOT "${CC_SANITIZER_OPTS}" STREQUAL "") AND (CMAKE_COMPILER_IS_GNUCC) AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.0"))
    message (WARNING "Compliler version is too old, sanitizers are disabled")
    set (CC_SANITIZER_OPTS)
endif ()

if (NOT CC_NO_VALGRIND AND ("${CC_SANITIZER_OPTS}" STREQUAL ""))
    if ("${CC_VALGRAND_EXE_NAME}" STREQUAL "")
        set (CC_VALGRAND_EXE_NAME "valgrind")
    endif ()
    find_program(VALGRIND_EXECUTABLE NAMES "${CC_VALGRAND_EXE_NAME}")
endif ()

if (TARGET cxxtest::cxxtest)
    test_func ("Fields")
    test_func ("Fields2")
    test_func ("Message")
    test_func ("MsgDataLayer")
    test_func ("MsgIdLayer")
    test_func ("MsgSizeLayer")
    test_func ("SyncPrefixLayer")
    test_func ("ChecksumLayer")
    test_func ("ChecksumPrefixLayer")
    test_func ("TransportValueLayer")
    test_func ("Util")
    test_func ("CustomMsgIdLayer")
    test_func ("CustomMsgSizeLayer")
    test_func ("Dispatch")
    test_func ("MsgFactory")
else ()
    message (Warning "Testing is enabled, but cxxtest hasn't been found!")
endif ()
