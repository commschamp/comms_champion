if (NOT CC_COMMS_CHAMPION_DEMO)
    return ()
endif ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory (plugin)