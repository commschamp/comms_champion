# No RPATH for binaries
set (CMAKE_SKIP_RPATH TRUE)

add_definitions(
    -DCC_BINDIR="${CMAKE_INSTALL_BINDIR}"
    -DCC_PLUGINDIR="${PLUGIN_INSTALL_REL_DIR}"
    -DCC_CONFIGDIR="${CONFIG_INSTALL_REL_DIR}"
)

add_subdirectory (cc_view)
add_subdirectory (cc_dump)
