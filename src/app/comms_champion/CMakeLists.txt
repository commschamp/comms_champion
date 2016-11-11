add_subdirectory (src)

if (UNIX)
    install(
        PROGRAMS script/comms_champion.sh
        DESTINATION ${BIN_INSTALL_DIR}
    )
endif ()  