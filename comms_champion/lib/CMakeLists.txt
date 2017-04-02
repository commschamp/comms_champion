add_subdirectory (src)

install (
    DIRECTORY "include/comms_champion"
    DESTINATION ${INC_INSTALL_DIR}
)

FILE(GLOB_RECURSE headers "include/*.h")
add_custom_target(comms_champion.headers SOURCES ${headers})
