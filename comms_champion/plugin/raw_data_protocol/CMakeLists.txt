include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory (cc_plugin)

install (
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/config/
    DESTINATION ${CONFIG_INSTALL_DIR}
)