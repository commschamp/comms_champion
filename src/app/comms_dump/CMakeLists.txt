add_subdirectory (src)

if (UNIX)
    install(
        PROGRAMS script/comms_dump.sh
        DESTINATION ${BIN_INSTALL_DIR}
    )
endif ()  