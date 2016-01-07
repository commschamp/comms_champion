add_subdirectory (comms)

if (CC_COMMS_LIB_ONLY)
    return ()
endif ()

add_subdirectory (comms_champion)