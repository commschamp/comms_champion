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
set (CC_TOOLS_QT_CMAKE_DIR "${_TWO_UP_DIR}/cc_tools_qt/cmake")

if ((NOT TARGET cc::comms) AND (EXISTS ${LIBCOMMS_CMAKE_DIR}/LibCommsConfig.cmake))
    include (${LIBCOMMS_CMAKE_DIR}/LibCommsConfig.cmake)
endif ()

if ((NOT TARGET cc::cc_tools_qt) AND (EXISTS ${CC_TOOLS_QT_CMAKE_DIR}/cc_tools_qtConfig.cmake))
    include (${CC_TOOLS_QT_CMAKE_DIR}/cc_tools_qtConfig.cmake)
endif ()

if ((NOT TARGET cc::comms_champion) AND (TARGET cc::cc_tools_qt))
    add_library(cc::comms_champion ALIAS cc::cc_tools_qt)
endif ()

set (CC_COMMS_CHAMPION_FOUND ${CC_TOOLS_QT_FOUND})
set (CC_INCLUDE_DIRS ${CC_TOOLS_QT_INCLUDE_DIRS})
set (CC_ROOT_DIR ${CC_TOOLS_QT_ROOT_DIR})
set (CC_PLUGIN_LIBRARIES ${CC_TOOLS_QT_PLUGIN_LIBRARIES})
set (CC_PLUGIN_LIBRARY_DIRS ${CC_TOOLS_QT_PLUGIN_LIBRARY_DIRS})
set (CC_PLUGIN_DIR ${CC_TOOLS_QT_PLUGIN_DIR})
set (CC_CMAKE_DIR ${CC_COMMS_CMAKE_DIR})
