cmake_minimum_required (VERSION 3.1)
project ("comms_champion")

# Available options

option (CC_INSTALL_COMMS_LIB "Install comms library headers." ON)
option (CC_BUILD_UNIT_TESTS "Build unittests." ${CC_INSTALL_COMMS_LIB})
option (CC_UNIT_TESTS_USE_SANITIZERS "Use sanitizers during unittests. Applicable when unittest are enabled." ON)
option (CC_BUILD_TOOLS_LIBRARY "Build tools library." ON)
option (CC_INSTALL_TOOLS_LIBRARY "Install tools library." ${CC_BUILD_TOOLS_LIBRARY})
option (CC_BUILD_TOOLS "Build tools binaries." ${CC_BUILD_TOOLS_LIBRARY})
option (CC_INSTALL_TOOLS "Install tools binaries." ${CC_BUILD_TOOLS})
option (CC_BUILD_DEMO_PROTOCOL "Build demo protocol plugin." OFF)
option (CC_INSTALL_DEMO_PROTOCOL "Install demo protocol plugin." ${CC_BUILD_DEMO_PROTOCOL})
option (CC_WARN_AS_ERR "Treat warning as error" ON)
option (CC_USE_CCACHE "Use ccache on UNIX systems if it's available" ON)
option (CC_SKIP_CXX_STANDARD_FORCING "Do NOT force C++ standard to C++11, use compiler's default one." ON)
option (CC_STATIC_RUNTIME "Enable/Disable static runtime" OFF)

# Extra variables
# CC_QT_DIR=dir - Directory of QT5 installation. Can be used to provide path to QT5 if
#   differs from system default installation path.
# CC_EXTERNALS_DIR - Directory where pull externals, defaults to 
#   ${PROJECT_SOURCE_DIR}/externals

# Deprecated options for backward compatibility, use the ones above for new builds.
# option (CC_COMMS_LIB_ONLY "Install only COMMS library, no other apps will be built." OFF)
# option (CC_LIBS_ONLY "Build and install COMMS and CommsChampion libraries, no other apps will be built." OFF)
# option (CC_NO_UNIT_TESTS "Disable unittests." OFF)
# option (CC_NO_WARN_AS_ERR "Do NOT treat warning as error" OFF)
# option (CC_ENABLE_DEMO_PROTOCOL "Enable demo protocol" OFF)
# option (CC_NO_CXX_STANDARD_FORCING "Disable forcing C++ standard to C++11 when no CMAKE_CXX_STANDARD is defined" OFF)
# option (CC_NO_CCACHE "Disable use of ccache on UNIX systems" OFF)

if (CMAKE_TOOLCHAIN_FILE AND EXISTS ${CMAKE_TOOLCHAIN_FILE})
    message(STATUS "Loading toolchain from ${CMAKE_TOOLCHAIN_FILE}")
endif()

if (NOT CC_EXTERNALS_DIR)
    set (CC_EXTERNALS_DIR "${PROJECT_SOURCE_DIR}/externals")
endif ()

set (CMAKE_SCRIPTS_DIR "${PROJECT_SOURCE_DIR}/cmake")

################################################

# Check usage of deprecated options and ovewrite the default values of proper ones
if ((DEFINED CC_COMMS_LIB_ONLY) AND (${CC_COMMS_LIB_ONLY}))
    set (CC_INSTALL_COMMS_LIB ON)
    set (CC_BUILD_TOOLS_LIBRARY OFF)
    set (CC_INSTALL_TOOLS_LIBRARY OFF)
    set (CC_BUILD_TOOLS OFF)
    set (CC_INSTALL_TOOLS OFF)
    set (CC_BUILD_DEMO_PROTOCOL OFF)
    set (CC_INSTALL_DEMO_PROTOCOL OFF)
endif ()

if ((DEFINED CC_LIBS_ONLY) AND (${CC_LIBS_ONLY}))
    set (CC_INSTALL_COMMS_LIB ON)
    set (CC_BUILD_TOOLS_LIBRARY ON)
    set (CC_INSTALL_TOOLS_LIBRARY ON)
    set (CC_BUILD_TOOLS OFF)
    set (CC_INSTALL_TOOLS OFF)
    set (CC_BUILD_DEMO_PROTOCOL OFF)
    set (CC_INSTALL_DEMO_PROTOCOL OFF)
endif ()

macro (cc_negate_option old_name new_name)
    if (DEFINED ${old_name})
        if (${old_name})
            set (${new_name} OFF)
        else ()
            set (${new_name} ON)
        endif ()
    endif ()
endmacro ()

cc_negate_option(CC_NO_UNIT_TESTS CC_BUILD_UNIT_TESTS)
cc_negate_option(CC_NO_WARN_AS_ERR CC_WARN_AS_ERR)
cc_negate_option(CC_NO_CCACHE CC_USE_CCACHE)

if (DEFINED CC_NO_CXX_STANDARD_FORCING)
    set (CC_SKIP_CXX_STANDARD_FORCING ${CC_NO_CXX_STANDARD_FORCING})
endif ()

if ((DEFINED CC_ENABLE_DEMO_PROTOCOL) AND (${CC_ENABLE_DEMO_PROTOCOL}))
    set (CC_BUILD_TOOLS_LIBRARY ON)
    set (CC_INSTALL_TOOLS_LIBRARY ON)
    set (CC_BUILD_DEMO_PROTOCOL ON)
    set (CC_INSTALL_DEMO_PROTOCOL ON)
endif ()

################################################

if ((NOT CMAKE_CXX_STANDARD) AND (NOT CC_SKIP_CXX_STANDARD_FORCING))
    set (CMAKE_CXX_STANDARD 11)
endif()

while (TRUE)
    if (NOT CC_BUILD_UNIT_TESTS)
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

################################################
# Compiler options
set (warn_opt)
if (CC_WARN_AS_ERR)
    set (warn_opt WARN_AS_ERR)
endif ()

set (static_runtime_opt)
if (CC_STATIC_RUNTIME)
    set (static_runtime_opt STATIC_RUNTIME)
endif ()

set (ccache_opt)
if ((UNIX) AND (CC_USE_CCACHE))
    set (ccache_opt USE_CCACHE)
endif ()

include (${CMAKE_SCRIPTS_DIR}/CC_Compile.cmake)
cc_compile(${warn_opt} ${static_runtime_opt} ${ccache_opt})
cc_msvc_force_warn_opt("/W4")

################################################

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

set (CC_COMMS_EXTERNAL_PROJ_SRC_DIR "${CC_EXTERNALS_DIR}/comms")
set (CC_COMMS_EXTERNAL_PROJ_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/comms_ext/build")
set (CC_COMMS_EXTERNAL_PROJ_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/comms_ext/install")

if (CC_INSTALL_COMMS_LIB)
    set (CC_COMMS_EXTERNAL_PROJ_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")
endif ()
   
add_subdirectory (comms)
add_subdirectory (comms_champion)


if (CC_INSTALL_TOOLS_LIBRARY)
    set (COMMS_CHAMPION_CMAKE_FILES
        ${PROJECT_SOURCE_DIR}/cmake/CommsChampionConfig.cmake
    ) 

    install (
        FILES ${COMMS_CHAMPION_CMAKE_FILES}
        DESTINATION ${LIB_INSTALL_DIR}/CommsChampion/cmake/
    )
endif()  