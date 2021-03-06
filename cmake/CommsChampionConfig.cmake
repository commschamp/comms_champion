# - Find CommsChampion
# Find the CommsChampion includes and libraries
# 
# Output is provided using the following variables
#  CC_COMMS_FOUND - Found COMMS library, use CC_INCLUDE_DIRS for includes
#  CC_COMMS_CHAMPION_FOUND - Found CommsChampion, use CC_INCLUDE_DIRS, CC_PLUGIN_LIBRARIES, CC_PLUGIN_DIR)
#  CC_INCLUDE_DIRS - Where to find all the headers
#  CC_ROOT_DIR - Root directory of CommsChampion installation
#  CC_PLUGIN_LIBRARIES - Libraries to link against when building a plugin
#  CC_PLUGIN_LIBRARY_DIRS - Where to find the libraries required to build plugin.
#  CC_PLUGIN_DIR - Directory where to install newly built plugin
#  CC_CMAKE_DIR - Directory containing installed cmake scripts
#
# Additional output targets
# cc::comms - Link target for COMMS library
# cc::comms_champion - Link target for "comms_champion" libarary. Use it in plugin development.

get_filename_component(_ONE_UP_DIR "${CMAKE_CURRENT_LIST_DIR}" PATH)
get_filename_component(_TWO_UP_DIR "${_ONE_UP_DIR}" PATH)
set (LIBCOMMS_CMAKE_DIR "${_TWO_UP_DIR}/LibComms/cmake")

if ((NOT TARGET cc::comms) AND (EXISTS ${LIBCOMMS_CMAKE_DIR}/LibCommsConfig.cmake))
    include (${LIBCOMMS_CMAKE_DIR}/LibCommsConfig.cmake)
endif ()

if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/comms_championExport.cmake)
    include (${CMAKE_CURRENT_LIST_DIR}/comms_championExport.cmake)
endif ()

if (NOT TARGET cc::comms_champion)
    return ()
endif ()

# Load information for each installed configuration.
file(GLOB config_files "${CMAKE_CURRENT_LIST_DIR}/CommsChampionConfig-*.cmake")
foreach(f ${config_files})
    include(${f})
endforeach()

get_filename_component (CC_INSTALL_LIB_PROJ_DIR ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)
get_filename_component (CC_INSTALL_LIB_DIR ${CC_INSTALL_LIB_PROJ_DIR} DIRECTORY)
get_filename_component (CC_ROOT_DIR ${CC_INSTALL_LIB_DIR} DIRECTORY)

find_path(CC_INCLUDE_DIR NAMES comms_champion/Message.h PATHS "${CC_ROOT_DIR}" PATH_SUFFIXES include)
find_library(CC_PLUGIN_LIBRARY  NAMES "comms_champion" PATHS "${CC_ROOT_DIR}" PATH_SUFFIXES lib)

if (MSVC)
    find_file(CC_NULL_SOCK_LIBRARY NAMES "null_socket.dll" PATHS "${CC_ROOT_DIR}/lib/CommsChampion" PATH_SUFFIXES plugin)
else ()
    find_library(CC_NULL_SOCK_LIBRARY  NAMES "null_socket" PATHS "${CC_ROOT_DIR}/lib/CommsChampion" PATH_SUFFIXES plugin)
endif ()

if (CC_PLUGIN_LIBRARY)
    get_filename_component  (CC_PLUGIN_LIBRARY_DIR ${CC_PLUGIN_LIBRARY} DIRECTORY)
endif ()

if (CC_NULL_SOCK_LIBRARY)
    get_filename_component  (CC_PLUGIN_DIR ${CC_NULL_SOCK_LIBRARY} DIRECTORY)
endif ()

set (CC_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR})

find_package(PackageHandleStandardArgs REQUIRED)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    CommsChampion
    REQUIRED_VARS 
        CC_ROOT_DIR CC_INCLUDE_DIR CC_PLUGIN_LIBRARY_DIR CC_PLUGIN_LIBRARY CC_CMAKE_DIR)

set (CC_COMMS_CHAMPION_FOUND ${CommsChampion_FOUND})        
    
if (CC_COMMS_CHAMPION_FOUND)
    set (CC_INCLUDE_DIRS ${CC_INCLUDE_DIR})
    set (CC_PLUGIN_LIBRARIES ${CC_PLUGIN_LIBRARY})
    set (CC_PLUGIN_LIBRARY_DIRS ${CC_PLUGIN_LIBRARY_DIR})
endif ()    

