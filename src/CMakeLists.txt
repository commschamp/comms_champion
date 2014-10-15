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

qt5_add_resources(COMMS_CHAMPION_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/ui.qrc)

 set (src
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

set (name "cc_protocol_analyser")
add_executable(${name} ${src} ${COMMS_CHAMPION_RESOURCES})
target_link_libraries(
    ${name}
    ${PROTOCOL_LIB_NAME} 
    Qt5::Qml Qt5::Quick Qt5::Widgets Qt5::Core)
    
