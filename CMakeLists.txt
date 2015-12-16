cmake_minimum_required (VERSION 2.8.11)
project ("comms_champion")

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
        "-Wmissing-include-dirs" "-Wnoexcept" "-Wold-style-cast" 
        "-Woverloaded-virtual" "-Wredundant-decls" "-Wshadow"
        "-Wno-unknown-pragmas"
    )
    # "-Wpedantic"   -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op  -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wundef -Wno-unused -Wno-variadic-macros -Wno-parentheses -fdiagnostics-show-option
endif ()


ext_add_cpp11_support()

add_subdirectory (src)


