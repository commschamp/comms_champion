# Find the QtWidgets library
find_package(Qt5Network REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/comms/include
    ${CMAKE_CURRENT_SOURCE_DIR}/app/comms_champion/include
)

set (INSTALL_DIR ${CMAKE_BINARY_DIR}/install)
set (LIB_INSTALL_DIR ${INSTALL_DIR}/lib)
set (BIN_INSTALL_DIR ${INSTALL_DIR}/bin)
set (INC_INSTALL_DIR ${INSTALL_DIR}/include)
set (PLUGIN_INSTALL_DIR ${INSTALL_DIR}/plugin)

add_subdirectory (lib)
add_subdirectory (app)
add_subdirectory (protocol)
