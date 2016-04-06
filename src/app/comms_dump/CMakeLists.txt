if (NOT CC_COMMS_DUMP)
    return ()
endif ()

add_subdirectory (src)

if (UNIX)
    install(
        PROGRAMS script/comms_dump.sh
        DESTINATION ${BIN_INSTALL_DIR}
    )
endif ()  