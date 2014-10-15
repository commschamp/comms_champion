# Find the QtWidgets library
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/protocol/include
)

set (PROTOCOL_LIB_NAME "cc_protocol")

add_subdirectory (lib)
add_subdirectory (app)
