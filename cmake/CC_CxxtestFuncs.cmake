
# This file contains several helper macros / functions to allow easy 
# unittesting with cxxtest.
# 
# Available functions / macros are:
#
# ******************************************************
# - Get "cxxtest" repository, executed in the configuration (cmake) stage.
#     cc_get_cxxtest(
#         INSTALL_PREFIX <externals_directory>
#         [TAG <cxxtest_tag>]
#         [REPO <cxxtest_repo>]
#         [TARGET <cxxtest_target>]
#         [NO_CTEST_INCLUDE]
#     )
# - INSTALL_PREFIX - A must have argument to specify parent directory 
#   where the cxxtest repo will reside.
# - TAG - Override the default tag to checkout.
# - REPO - Override the default repository of the cxxtest.
# - TARGET - Override the default (cxxtest::cxxtest) target name for the cxxtest.
# - NO_CTEST_INCLUDE - Dont include "CTest" at the end
#
# ******************************************************
# - Add cxxtest based unittest
#     cc_cxxtest_add_test(
#         NAME <test_name>
#         SRC <src1> [<src2> <src3> ...]
#         [COMMS_TARGET <comms_lib_target>]
#         [CXXTEST_TARGET <cxxtest_target]
#         [VALGRIND_EXECUTABLE <path_to_valgrind>]
#         [NO_COMMS_LIB_DEP]
#     )
# - NAME - A must have argument to provide name for the test.
# - SRC - A list of test sources.
# - COMMS_TARGET - Override the default cmake target for the comms library, defaults to
#   cc::comms.
# - CXXTEST_TARGET - Override the default cmake target for the cxxtest, default to
#   cxxtest::cxxtest
# - VALGRIND_EXECUTABLE - Path to "valgrind" executable if valgrind tests to be added.
# - NO_COMMS_LIB_DEP - Exlcude depenedency on comms library

set (CC_CXXTEST_DEFAULT_TAG "4.4")
set (CC_CXXTEST_DEFAULT_REPO "https://github.com/CxxTest/cxxtest.git")
set (CC_CXXTEST_DEFAULT_TARGET "cxxtest::cxxtest")
set (CC_CXXTEST_DEFAULT_COMMS_LIB_TARGET "cc::comms")
set (CC_CXXTEST_VALGRIND_RUN_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/CC_RunWithValgrindScript.cmake")

macro (cc_get_cxxtest)
    while (TRUE)
        set (_prefix CC_CXXTEST_TGT)
        set (_options NO_CTEST_INCLUDE)
        set (_oneValueArgs INSTALL_PREFIX TAG REPO TARGET)
        set (_mutiValueArgs)
        cmake_parse_arguments(${_prefix} "${_options}" "${_oneValueArgs}" "${_mutiValueArgs}" ${ARGN})

        if ("${CC_CXXTEST_TGT_INSTALL_PREFIX}" STREQUAL "")
            message (FATAL_ERROR "The INSTALL_PREFIX parameter is not provided")
        endif ()

        if (NOT CC_CXXTEST_TGT_TAG)
            set (CC_CXXTEST_TGT_TAG ${CC_CXXTEST_DEFAULT_TAG})
        endif ()

        if (NOT CC_CXXTEST_TGT_REPO)
            set (CC_CXXTEST_TGT_REPO ${CC_CXXTEST_DEFAULT_REPO})
        endif ()

       if (NOT CC_CXXTEST_TGT_TARGET)
            set (CC_CXXTEST_TGT_TARGET ${CC_CXXTEST_DEFAULT_TARGET})
        endif ()    

        if (TARGET ${CC_CXXTEST_TGT_TARGET})
            message (WARNING "Target ${CC_CXXTEST_TGT_TARGET} is already defined")
            break ()
        endif ()    

        if (NOT GIT_FOUND)
            find_package (Git REQUIRED)
        endif ()

        set (cxxtest_dir "${CC_CXXTEST_TGT_INSTALL_PREFIX}/cxxtest")
        set (cxxtest_stamp "${cxxtest_dir}/cxxtest.stamp")
        if (EXISTS ${cxxtest_stamp})
            execute_process (
                COMMAND ${GIT_EXECUTABLE} checkout -q ${CC_CXXTEST_TGT_TAG}
                WORKING_DIRECTORY ${cxxtest_dir}
            )
        else ()
            message (STATUS "${cxxtest_stamp} does NOT exist")
            execute_process (
                COMMAND ${CMAKE_COMMAND} -E remove_directory "${cxxtest_dir}"
            )
            
            execute_process (
                COMMAND ${CMAKE_COMMAND} -E make_directory "${CC_CXXTEST_TGT_INSTALL_PREFIX}"
            )
            
            execute_process (
                COMMAND ${GIT_EXECUTABLE} clone -b ${CC_CXXTEST_TGT_TAG} --depth 1 ${CC_CXXTEST_TGT_REPO}
                WORKING_DIRECTORY ${CC_CXXTEST_TGT_INSTALL_PREFIX}
                RESULT_VARIABLE git_result
            )
        
            if (NOT "${git_result}" STREQUAL "0")
                message (WARNING "git clone/checkout failed")
                break()
            endif ()
            
            execute_process (
                COMMAND ${CMAKE_COMMAND} -E touch "${cxxtest_stamp}"
            )
        endif ()
        
        set (CXXTEST_INCLUDE_DIR "${cxxtest_dir}")
        set (CXXTEST_PYTHON_TESTGEN_EXECUTABLE "${CXXTEST_INCLUDE_DIR}/bin/cxxtestgen")
        
        find_package (CxxTest REQUIRED)

        set (local_cxxtest_name "cc_cxxtest")
        add_library("cc_cxxtest" INTERFACE)
        add_library(${CC_CXXTEST_TGT_TARGET} ALIAS ${local_cxxtest_name})
        target_compile_options(${local_cxxtest_name} INTERFACE
            $<$<CXX_COMPILER_ID:MSVC>:/wd5055>
            $<$<CXX_COMPILER_ID:GNU>:-Wno-old-style-cast -Wno-shadow>
            $<$<CXX_COMPILER_ID:Clang>:-Wno-unknown-warning-option -Wno-deprecated-enum-float-conversion>
        ) 

        if ((CMAKE_COMPILER_IS_GNUCC) AND (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "7.9"))
            target_compile_options(${local_cxxtest_name} INTERFACE
                $<$<CXX_COMPILER_ID:GNU>:-Wno-ignored-qualifiers>
            ) 
        endif ()    

        target_include_directories(${local_cxxtest_name} INTERFACE ${CXXTEST_INCLUDE_DIR})

        if (CC_CXXTEST_TGT_NO_CTEST_INCLUDE)
            break()
        endif ()

        include(CTest)
        break ()
    endwhile ()
endmacro ()

function (cc_cxxtest_add_test)
    set (_prefix CC_CXXTEST_TEST)
    set (_options ADD_VALGRIND_TEST NO_COMMS_LIB_DEP)
    set (_oneValueArgs NAME COMMS_TARGET CXXTEST_TARGET VALGRIND_EXECUTABLE)
    set (_mutiValueArgs SRC)
    cmake_parse_arguments(${_prefix} "${_options}" "${_oneValueArgs}" "${_mutiValueArgs}" ${ARGN})

    set (runner "${CC_CXXTEST_TEST_NAME}_CxxtestRunner.cpp")

    if ((NOT CC_CXXTEST_TEST_COMMS_TARGET) AND (NOT CC_CXXTEST_TEST_NO_COMMS_LIB_DEP))
        set (CC_CXXTEST_TEST_COMMS_TARGET ${CC_CXXTEST_DEFAULT_COMMS_LIB_TARGET})
    endif ()     

    if (NOT CC_CXXTEST_TEST_CXXTEST_TARGET)
        set (CC_CXXTEST_TEST_CXXTEST_TARGET ${CC_CXXTEST_DEFAULT_TARGET})
    endif () 
    
    CXXTEST_ADD_TEST (${CC_CXXTEST_TEST_NAME} ${runner} ${CC_CXXTEST_TEST_SRC})
    target_link_libraries(${CC_CXXTEST_TEST_NAME} ${CC_CXXTEST_TEST_COMMS_TARGET} ${CC_CXXTEST_TEST_CXXTEST_TARGET})

    if (NOT CC_CXXTEST_TEST_VALGRIND_EXECUTABLE)
        return ()
    endif ()

    add_test(NAME "${CC_CXXTEST_TEST_NAME}_valgrind"
            COMMAND ${CMAKE_COMMAND} 
            -DTEST_EXECUTABLE=$<TARGET_FILE:${CC_CXXTEST_TEST_NAME}>
            -DVALGRIND_EXECUTABLE=${CC_CXXTEST_TEST_VALGRIND_EXECUTABLE}
            -P ${CC_CXXTEST_VALGRIND_RUN_SCRIPT}) 
    
endfunction ()