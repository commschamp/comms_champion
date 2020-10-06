# Find COMMS library from CommsChampion Ecosystem
# 
# Output is provided using the following variables
#  CC_COMMS_FOUND - Found COMMS library, use CC_COMMS_INCLUDE_DIRS for includes
#  CC_COMMS_INCLUDE_DIRS - Where to find all the headers
#
# Additional output targets
# cc::comms - Link target for COMMS library

if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/commsExport.cmake)
    include (${CMAKE_CURRENT_LIST_DIR}/commsExport.cmake)
endif ()

# Load information for each installed configuration.
file(GLOB config_files "${CMAKE_CURRENT_LIST_DIR}/LibCommsConfig-*.cmake")
foreach(f ${config_files})
    include(${f})
endforeach()

if (NOT TARGET cc::comms)
    return ()
endif ()

get_target_property(CC_COMMS_INCLUDE_DIRS cc::comms INTERFACE_INCLUDE_DIRECTORIES)

find_package(PackageHandleStandardArgs REQUIRED)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    LibComms
    REQUIRED_VARS CC_COMMS_INCLUDE_DIRS)

set (CC_COMMS_FOUND ${LibComms_FOUND})

if (CC_COMMS_FOUND)
    set (CC_COMMS_INCLUDE_DIR ${CC_COMMS_INCLUDE_DIRS})
endif ()    



