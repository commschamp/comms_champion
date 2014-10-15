# Find the QtWidgets library
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/comms_champion/include
)

set (COMMS_CHAMPION_LIB_NAME "comms_champion")

add_subdirectory (lib)

qt5_add_resources(COMMS_CHAMPION_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/ui.qrc)

 set (src
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

set (comms_champion_bin_name "comms_champion")
set (comms_champion_bin_tgt ${comms_champion_bin_name}.bin)
add_executable(${comms_champion_bin_tgt} ${src} ${COMMS_CHAMPION_RESOURCES})
target_link_libraries(
    ${comms_champion_bin_tgt}
    ${COMMS_CHAMPION_LIB_NAME} 
    Qt5::Qml Qt5::Quick Qt5::Widgets Qt5::Core)
    
set_target_properties(${comms_champion_bin_tgt} PROPERTIES OUTPUT_NAME ${comms_champion_bin_name}) 
