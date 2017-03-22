cmake_minimum_required (VERSION 3.0)
project ("comms_champion")

option (CC_COMMS_LIB_ONLY "Install only COMMS library, no other apps will be built." OFF)
option (CC_STATIC_RUNTIME "Enable/Disable static runtime" OFF)
option (CC_NO_UNIT_TESTS "Disable unittests." OFF)
option (CC_NO_WARN_AS_ERR "Do NOT treat warning as error" OFF)

if (CMAKE_TOOLCHAIN_FILE AND EXISTS ${CMAKE_TOOLCHAIN_FILE})
    message(STATUS "Loading toolchain from ${CMAKE_TOOLCHAIN_FILE}")
endif()

while (TRUE)
    if (CC_NO_UNIT_TESTS)
        message (STATUS "Unittests are disabled")
        break()
    endif ()
    
    if (CMAKE_CROSSCOMPILING)
        message (STATUS "Not building unittests when cross-compiling")
        break()
    endif ()
    
    find_package (Git)
    if (NOT GIT_FOUND)
        message (WARNING "Git is required to check out cxxtest")
        break()
    endif ()
    
    set (cxx_tag "4.4")
    set (cxxtest_stamp "${CMAKE_BINARY_DIR}/cxxtest.stamp")
    if ((EXISTS ${cxxtest_stamp}) AND (EXISTS "${CMAKE_BINARY_DIR}/cxxtest"))
        execute_process (
            COMMAND ${GIT_EXECUTABLE} checkout ${cxx_tag}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/cxxtest
        )
    else ()
        execute_process (
            COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/cxxtest"
        )
        
        execute_process (
            COMMAND ${GIT_EXECUTABLE} clone -b ${cxx_tag} --depth 1 https://github.com/CxxTest/cxxtest.git
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
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
    
    set (CXXTEST_INCLUDE_DIR "${CMAKE_BINARY_DIR}/cxxtest")
    set (CXXTEST_PYTHON_TESTGEN_EXECUTABLE "${CXXTEST_INCLUDE_DIR}/bin/cxxtestgen")
    
    find_package (CxxTest)
    if (NOT CXXTEST_FOUND)
        message (WARNING "Wrong cxxtest paths, must be fixed")
        break()
    endif ()
    
    enable_testing ()
    break()
endwhile ()

if (CMAKE_COMPILER_IS_GNUCC)
    set (extra_flags_list
        "--std=c++11"
        "-Wall" "-Wextra" "-Wcast-align" "-Wcast-qual" "-Wctor-dtor-privacy"
        "-Wmissing-include-dirs" "-Wnoexcept" "-Wlogical-op" "-Wstrict-null-sentinel"
        "-Woverloaded-virtual" "-Wredundant-decls" "-Wshadow" "-Wundef" "-Wunused"
        "-Wno-unknown-pragmas" "-fdiagnostics-show-option"
    )
    
    if (NOT CC_NO_WARN_AS_ERR)
        list (APPEND extra_flags_list "-Werror")
    endif ()
    
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

if ((UNIX) AND (NOT CC_NO_CCACHE))
    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    endif(CCACHE_FOUND)
endif ()    

set (COMMS_CHAMPION_LIB_NAME "comms_champion")
set (COMMS_CHAMPION_LIB_TGT "${COMMS_CHAMPION_LIB_NAME}.lib")

if (NOT "${CC_QT_DIR}" STREQUAL "")
    list (APPEND CMAKE_PREFIX_PATH ${CC_QT_DIR})
    link_directories ("${CC_QT_DIR}/lib")
    link_directories ("${CC_QT_DIR}/plugins/platforms")
endif ()

set (INSTALL_DIR ${CMAKE_BINARY_DIR}/install)
if (NOT "${CC_INSTALL_DIR}" STREQUAL "")
    set (INSTALL_DIR "${CC_INSTALL_DIR}")
endif ()

include(GNUInstallDirs)
set (LIB_INSTALL_DIR ${INSTALL_DIR}/${CMAKE_INSTALL_LIBDIR})
set (BIN_INSTALL_DIR ${INSTALL_DIR}/${CMAKE_INSTALL_BINDIR})
set (INC_INSTALL_DIR ${INSTALL_DIR}/${CMAKE_INSTALL_INCLUDEDIR})
set (PLUGIN_INSTALL_REL_DIR ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/plugin)
set (PLUGIN_INSTALL_DIR ${INSTALL_DIR}/${PLUGIN_INSTALL_REL_DIR})
set (DATA_INSTALL_REL_DIR ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})
set (DATA_INSTALL_DIR ${INSTALL_DIR}/${DATA_INSTALL_REL_DIR})
set (DOC_INSTALL_DIR ${DATA_INSTALL_DIR}/doc)
set (ICON_INSTALL_DIR ${DATA_INSTALL_DIR}/icon)
set (CONFIG_INSTALL_REL_DIR config)
set (CONFIG_INSTALL_DIR ${INSTALL_DIR}/${CONFIG_INSTALL_REL_DIR})
set (CMAKE_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/cmake)

install (DIRECTORY cmake DESTINATION ${LIB_INSTALL_DIR}/${PROJECT_NAME})

include_directories (
    BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}/comms/include
)

add_subdirectory (comms)

if (CC_COMMS_LIB_ONLY)
    return ()
endif ()

add_subdirectory (comms_champion)

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/comms_champion/lib/include
)

add_subdirectory (demo)

