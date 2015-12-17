cmake_minimum_required (VERSION 2.8.11)
project ("comms_champion")

option (CC_COMMS_LIB_ONLY "Install only COMMS library, no other apps will be built." OFF)
option (CC_COMMS_UNIT_TESTS "Build unittests of COMMS library" OFF)
option (CC_COMMS_CHAMPION "Build CommsChampion application" ON)
option (CC_COMMS_CHAMPION_DEMO "Build demo protocol plugin" OFF)

set (EXTERNALS_DIR "${CMAKE_SOURCE_DIR}/external")
find_package (Git REQUIRED)
 
execute_process (
    COMMAND ${GIT_EXECUTABLE} submodule update --init
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
 
set (CMAKE_MODULE_PATH "${EXTERNALS_DIR}/cmake")         
include (AllExtras)

if (NOT NO_UNITTESTS)
    set (CXXTEST_INCLUDE_DIR "${EXTERNALS_DIR}/cxxtest")
    set (CXXTEST_PYTHON_TESTGEN_EXECUTABLE "${CXXTEST_INCLUDE_DIR}/bin/cxxtestgen")
    
    find_package (CxxTest)
    if (CXXTEST_FOUND)
        enable_testing ()
    endif ()
endif ()

if (CMAKE_COMPILER_IS_GNUCC)
    ext_add_c_cxx_flags(
        "-Wall" "-Wextra" "-Werror" "-Wcast-align" "-Wcast-qual" "-Wctor-dtor-privacy"
        "-Wmissing-include-dirs" "-Wnoexcept" "-Wlogical-op" "-Wstrict-null-sentinel"
        "-Woverloaded-virtual" "-Wredundant-decls" "-Wshadow" "-Wundef" "-Wunused"
        "-Wno-unknown-pragmas" "-fdiagnostics-show-option"
    )
endif ()

ext_add_cpp11_support()

add_subdirectory (src)


