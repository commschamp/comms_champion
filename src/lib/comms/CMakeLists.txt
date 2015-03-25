add_subdirectory (test)

install (
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/comms
    DESTINATION ${INC_INSTALL_DIR}
)