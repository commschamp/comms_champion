
function (test_func_catch2 test_suite_name)
    set (src "${CMAKE_CURRENT_SOURCE_DIR}/${test_suite_name}.cpp")

    set (name "${COMPONENT_NAME}.${test_suite_name}Catch2")

    add_executable(${name} ${src})
    target_link_libraries(${name} cc::comms Catch2::Catch2)

    target_compile_options(${name} PRIVATE
      $<$<CXX_COMPILER_ID:MSVC>:/bigobj>
      $<$<CXX_COMPILER_ID:Clang>:-DCATCH_CONFIG_NO_CPP17_BYTE=1>
    )    

    add_test(NAME ${name} COMMAND $<TARGET_FILE:${name}>)        

    if (VALGRIND_EXECUTABLE)
        add_test(NAME "${name}_valgrind"
             COMMAND ${CMAKE_COMMAND} 
                -DTEST_EXECUTABLE=$<TARGET_FILE:${name}>
                -DVALGRIND_EXECUTABLE=${VALGRIND_EXECUTABLE}
                -P ${CMAKE_CURRENT_SOURCE_DIR}/ScriptRunUnitTestWithValgrind.cmake) 
    endif ()
endfunction ()

#################################################################

test_func_catch2("FieldsTest")
test_func_catch2("FieldsTest2")
test_func_catch2("MessageTest")
