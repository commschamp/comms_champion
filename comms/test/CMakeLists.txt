# In order to run the unittests the following conditions must be true:
#   - find_package (CxxTest) was exectued, CXXTEST_FOUND is defined and has true value.

if (NOT CXXTEST_FOUND)
    return ()
endif ()    

set (COMPONENT_NAME "comms")

#################################################################

function (test_func test_suite_name)
    set (tests "${CMAKE_CURRENT_SOURCE_DIR}/${test_suite_name}.th")

    set (name "${COMPONENT_NAME}.${test_suite_name}Test")

    set (runner "${test_suite_name}TestRunner.cpp")
    
    CXXTEST_ADD_TEST (${name} ${runner} ${tests} ${extra_sources})
    target_link_libraries(${name} cc::comms)
    
    if (VALGRIND_EXECUTABLE)
        add_test(NAME "${name}_valgrind"
             COMMAND ${CMAKE_COMMAND} 
                -DTEST_EXECUTABLE=$<TARGET_FILE:${name}>
                -DVALGRIND_EXECUTABLE=${VALGRIND_EXECUTABLE}
                -P ${CMAKE_CURRENT_SOURCE_DIR}/ScriptRunUnitTestWithValgrind.cmake) 
    endif ()
    
endfunction ()

#################################################################

function (test_fields)
    test_func ("Fields")
endfunction ()

#################################################################

function (test_fields2)
    test_func ("Fields2")
endfunction ()

#################################################################

function (test_message)
    test_func ("Message")
endfunction ()

#################################################################

function (test_msg_data_layer)
    test_func ("MsgDataLayer")
endfunction ()

#################################################################

function (test_msg_id_layer)
    test_func ("MsgIdLayer")
endfunction ()

#################################################################

function (test_msg_size_layer)
    test_func ("MsgSizeLayer")
endfunction ()

#################################################################

function (test_sync_prefix_layer)
    test_func ("SyncPrefixLayer")
endfunction ()

#################################################################

function (test_checksum_layer)
    test_func ("ChecksumLayer")
endfunction ()

#################################################################

function (test_checksum_prefix_layer)
    test_func ("ChecksumPrefixLayer")
endfunction ()

#################################################################

function (test_transport_value_layer)
    test_func ("TransportValueLayer")
endfunction ()

#################################################################

function (test_util)
    test_func ("Util")
endfunction ()

#################################################################

function (test_custom_msg_id_layer)
    test_func ("CustomMsgIdLayer")
endfunction ()

#################################################################

function (test_custom_msg_size_layer)
    test_func ("CustomMsgSizeLayer")
endfunction ()

#################################################################

include_directories ("${CXXTEST_INCLUDE_DIR}")

if (CMAKE_COMPILER_IS_GNUCC)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-old-style-cast -Wno-shadow")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "7.9")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-ignored-qualifiers")
    endif ()    
endif ()

if (NOT CC_NO_VALGRIND)
    if ("${CC_VALGRAND_EXE_NAME}" STREQUAL "")
        set (CC_VALGRAND_EXE_NAME "valgrind")
    endif ()
    find_program(VALGRIND_EXECUTABLE NAMES "${CC_VALGRAND_EXE_NAME}")
endif ()

test_fields()
test_fields2()
test_message()
test_msg_data_layer()
test_msg_id_layer()
test_msg_size_layer()
test_sync_prefix_layer()
test_checksum_layer()
test_checksum_prefix_layer()
test_transport_value_layer()
test_util()
test_custom_msg_id_layer()
test_custom_msg_size_layer()
