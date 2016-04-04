cmake_minimum_required (VERSION 2.8.11)
project ("comms_champion")

option (CC_COMMS_LIB_ONLY "Install only COMMS library, no other apps will be built." OFF)
option (CC_COMMS_UNIT_TESTS "Build unittests of COMMS library" OFF)
option (CC_COMMS_CHAMPION "Build CommsChampion application" ON)
option (CC_COMMS_CHAMPION_DEMO "Build demo protocol plugin" ON)
option (CC_STATIC_RUNTIME "Enable/Disable static runtime" OFF)

set (EXTERNALS_DIR "${CMAKE_SOURCE_DIR}/external")

find_package (Git)

if (GIT_FOUND)
    execute_process (
    COMMAND ${GIT_EXECUTABLE} submodule update --init
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif () 

if (CC_COMMS_UNIT_TESTS)
    set (CXXTEST_INCLUDE_DIR "${EXTERNALS_DIR}/cxxtest")
    set (CXXTEST_PYTHON_TESTGEN_EXECUTABLE "${CXXTEST_INCLUDE_DIR}/bin/cxxtestgen")
    
    find_package (CxxTest)
    if (CXXTEST_FOUND)
        enable_testing ()
    endif ()
endif ()

if (CMAKE_COMPILER_IS_GNUCC)
    set (extra_flags_list
        "--std=c++11"
        "-Wall" "-Wextra" "-Werror" "-Wcast-align" "-Wcast-qual" "-Wctor-dtor-privacy"
        "-Wmissing-include-dirs" "-Wnoexcept" "-Wlogical-op" "-Wstrict-null-sentinel"
        "-Woverloaded-virtual" "-Wredundant-decls" "-Wshadow" "-Wundef" "-Wunused"
        "-Wno-unknown-pragmas" "-fdiagnostics-show-option"
    )
    
    string(REPLACE ";" " " extra_flags "${extra_flags_list}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${extra_flags}")
    
    if (CC_STATIC_RUNTIME)
        SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
    endif ()
elseif (MSVC)

    add_definitions( "/wd4503" "/wd4309" "/wd4267" "-D_SCL_SECURE_NO_WARNINGS")
    if (CC_STATIC_RUNTIME)
        foreach(flag_var 
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
            if(${flag_var} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
            endif()
        endforeach()
    endif ()
endif ()

set (COMMS_CHAMPION_LIB_NAME "comms_champion")
set (COMMS_CHAMPION_LIB_TGT "${COMMS_CHAMPION_LIB_NAME}.lib")

if (NOT "${CC_QT_DIR}" STREQUAL "")
    list (APPEND CMAKE_PREFIX_PATH ${CC_QT_DIR})
    link_directories ("${CC_QT_DIR}/lib")
    link_directories ("${CC_QT_DIR}/plugins/platforms")
endif ()

set (INSTALL_DIR ${CMAKE_BINARY_DIR}/install)
set (LIB_INSTALL_DIR ${INSTALL_DIR}/lib)
set (BIN_INSTALL_DIR ${INSTALL_DIR}/bin)
set (INC_INSTALL_DIR ${INSTALL_DIR}/include)
set (PLUGIN_INSTALL_DIR ${INSTALL_DIR}/plugin)
set (DOC_INSTALL_DIR ${INSTALL_DIR}/doc)
set (ICON_INSTALL_DIR ${INSTALL_DIR}/icon)

install (DIRECTORY cmake DESTINATION ${INSTALL_DIR})
add_subdirectory (src)
