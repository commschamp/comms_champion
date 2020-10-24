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

    set (satinizer_opts)
    if (CC_UNIT_TESTS_USE_SANITIZERS)
        set (satinizer_opts -fno-omit-frame-pointer -fno-sanitize-recover=undefined -fsanitize=undefined)
    endif ()

    target_compile_options(${name} PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wno-old-style-cast ${satinizer_opts}>
        $<$<CXX_COMPILER_ID:Clang>:-Wno-old-style-cast ${satinizer_opts}>
    ) 

    if (CC_UNIT_TESTS_USE_SANITIZERS)
        target_link_options(${name} PRIVATE
            $<$<CXX_COMPILER_ID:GNU>:${satinizer_opts}>
            $<$<CXX_COMPILER_ID:Clang>:${satinizer_opts}>
        )     
    endif ()

endfunction ()

#################################################################

if (NOT CC_NO_VALGRIND)
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
