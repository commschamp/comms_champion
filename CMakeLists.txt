cmake_minimum_required (VERSION 3.1)
project ("comms_champion")

# Available options
option (CC_COMMS_LIB_ONLY "Install only COMMS library, no other apps will be built." OFF)
option (CC_LIBS_ONLY "Build and install COMMS and CommsChampion libraries, no other apps will be built." OFF)
option (CC_STATIC_RUNTIME "Enable/Disable static runtime" OFF)
option (CC_NO_UNIT_TESTS "Disable unittests." OFF)
option (CC_NO_WARN_AS_ERR "Do NOT treat warning as error" OFF)
option (CC_ENABLE_DEMO_PROTOCOL "Enable demo protocol" OFF)
option (CC_NO_CXX_STANDARD_FORCING "Disable forcing C++ standard to C++11 when no CMAKE_CXX_STANDARD is defined" OFF)

# Extra variables
# CC_QT_DIR=dir - Directory of QT5 installation. Can be used to provide path to QT5 if
#   differs from system default installation path.
# CC_EXTERNALS_DIR - Directory where pull externals, defaults to 
#   ${PROJECT_SOURCE_DIR}/externals

if (CMAKE_TOOLCHAIN_FILE AND EXISTS ${CMAKE_TOOLCHAIN_FILE})
    message(STATUS "Loading toolchain from ${CMAKE_TOOLCHAIN_FILE}")
endif()

if ((NOT CMAKE_CXX_STANDARD) AND (NOT CC_NO_CXX_STANDARD_FORCING))
    set (CMAKE_CXX_STANDARD 11)
endif()

if (NOT CC_EXTERNALS_DIR)
    set (CC_EXTERNALS_DIR "${PROJECT_SOURCE_DIR}/externals")
endif ()

set (CMAKE_SCRIPTS_DIR "${PROJECT_SOURCE_DIR}/cmake")
while (TRUE)
    if (CC_NO_UNIT_TESTS)
        message (STATUS "Unittests are disabled")
        break()
    endif ()
    
    if (CMAKE_CROSSCOMPILING)
        message (STATUS "Not building unittests when cross-compiling")
        break()
    endif ()

    include (${CMAKE_SCRIPTS_DIR}/CC_CxxtestFuncs.cmake)
    cc_get_cxxtest(INSTALL_PREFIX ${CC_EXTERNALS_DIR})
    break()
endwhile ()

if ((CMAKE_COMPILER_IS_GNUCC) OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
    set (extra_flags_list
        "-Wall" "-Wextra" "-Wcast-align" "-Wcast-qual" "-Wctor-dtor-privacy"
        "-Wmissing-include-dirs"
        "-Woverloaded-virtual" "-Wredundant-decls" "-Wshadow" "-Wundef" "-Wunused"
        "-Wno-unknown-pragmas" "-fdiagnostics-show-option"
    )

    if (CMAKE_COMPILER_IS_GNUCC)
        list (APPEND extra_flags_list
            "-Wnoexcept" "-Wlogical-op" "-Wstrict-null-sentinel"
        )

        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.8")
            list (APPEND extra_flags_list
                "-Wno-pragmas" "-Wno-type-limits"
            )
        endif()

    endif ()

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        list (APPEND extra_flags_list "-Wno-dangling-field -Wno-unused-command-line-argument")
    endif ()
    
    if (NOT CC_NO_WARN_AS_ERR)
        list (APPEND extra_flags_list "-Werror")
    endif ()
    
    string(REPLACE ";" " " extra_flags "${extra_flags_list}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${extra_flags}")
    
    if (CC_STATIC_RUNTIME)
        SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
    endif ()
elseif (MSVC)

    if (NOT CC_NO_WARN_AS_ERR)
        add_definitions("/WX")
    endif ()

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

if (NOT "${CC_QT_DIR}" STREQUAL "")
    list (APPEND CMAKE_PREFIX_PATH ${CC_QT_DIR})
    link_directories ("${CC_QT_DIR}/lib")
    link_directories ("${CC_QT_DIR}/plugins/platforms")
endif ()

include(GNUInstallDirs)
set (INSTALL_NAME "CommsChampion")
set (LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
set (BIN_INSTALL_DIR ${CMAKE_INSTALL_BINDIR})
set (INC_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR})
set (PLUGIN_INSTALL_REL_DIR ${CMAKE_INSTALL_LIBDIR}/${INSTALL_NAME}/plugin)
set (PLUGIN_INSTALL_DIR ${PLUGIN_INSTALL_REL_DIR})
set (DATA_INSTALL_REL_DIR ${CMAKE_INSTALL_DATAROOTDIR}/${INSTALL_NAME})
set (DATA_INSTALL_DIR ${DATA_INSTALL_REL_DIR})
set (DOC_INSTALL_DIR ${CMAKE_INSTALL_DATAROOTDIR}/doc)
set (ICON_INSTALL_DIR ${DATA_INSTALL_DIR}/icon)
set (CONFIG_INSTALL_REL_DIR ${CMAKE_INSTALL_DATADIR}/${INSTALL_NAME})
set (CONFIG_INSTALL_DIR ${CONFIG_INSTALL_REL_DIR})

file (READ "${PROJECT_SOURCE_DIR}/comms/include/comms/version.h" version_file)
string (REGEX MATCH "COMMS_MAJOR_VERSION ([0-9]*)U*" _ ${version_file})
set (major_ver ${CMAKE_MATCH_1})
string (REGEX MATCH "COMMS_MINOR_VERSION ([0-9]*)U*" _ ${version_file})
set (minor_ver ${CMAKE_MATCH_1})
string (REGEX MATCH "COMMS_PATCH_VERSION ([0-9]*)U*" _ ${version_file})
set (patch_ver ${CMAKE_MATCH_1})
set (COMMS_VERSION "${major_ver}.${minor_ver}.${patch_ver}")


include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/LibCommsConfigVersion.cmake
    VERSION ${COMMS_VERSION}
    COMPATIBILITY AnyNewerVersion)

write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/CommsChampionConfigVersion.cmake
    VERSION ${COMMS_VERSION}
    COMPATIBILITY AnyNewerVersion)
    
set (LIB_COMMS_CMAKE_FILES
    ${PROJECT_SOURCE_DIR}/cmake/CC_CxxtestFuncs.cmake
    ${PROJECT_SOURCE_DIR}/cmake/CC_DefineExternalProjectTargets.cmake
    ${PROJECT_SOURCE_DIR}/cmake/CC_DocCleanupScript.cmake
    ${PROJECT_SOURCE_DIR}/cmake/CC_RunWithValgrindScript.cmake
    ${PROJECT_SOURCE_DIR}/cmake/LibCommsConfig.cmake
    ${PROJECT_SOURCE_DIR}/cmake/DefineExternalProjectTargets.cmake
    ${CMAKE_BINARY_DIR}/LibCommsConfigVersion.cmake
) 

set (COMMS_CHAMPION_CMAKE_FILES
    ${LIB_COMMS_CMAKE_FILES}
    ${PROJECT_SOURCE_DIR}/cmake/CC_DeployQt5Script.cmake
    ${PROJECT_SOURCE_DIR}/cmake/CC_GenWinAppStartBat.cmake
    ${PROJECT_SOURCE_DIR}/cmake/CommsChampionConfig.cmake
)    
   
install (
    FILES ${LIB_COMMS_CMAKE_FILES}
    DESTINATION ${LIB_INSTALL_DIR}/LibComms/cmake/
)

install (
    FILES ${COMMS_CHAMPION_CMAKE_FILES}
    DESTINATION ${LIB_INSTALL_DIR}/CommsChampion/cmake/
)


add_subdirectory (comms)

if (CC_COMMS_LIB_ONLY)
    return ()
endif ()

add_subdirectory (comms_champion)

if (CC_LIBS_ONLY)
    return ()
endif ()

add_subdirectory (demo)

if (WIN32 AND CC_QT_DIR)
    add_custom_target ("deploy_qt"
        COMMAND ${CMAKE_COMMAND} -DCC_QT_DIR=${CC_QT_DIR}
            -DCC_BIN_DIR=${BIN_INSTALL_DIR} -DCC_PLUGIN_DIR=${PLUGIN_INSTALL_DIR}
            -P ${PROJECT_SOURCE_DIR}/cmake/CC_DeployQt5Script.cmake
    )
endif ()
