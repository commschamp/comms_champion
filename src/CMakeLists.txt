
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Widgets REQUIRED)

qt5_add_resources(COMMS_CHAMPION_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/ui.qrc)

file (GLOB_RECURSE srcs ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(comms_champion ${srcs} ${COMMS_CHAMPION_RESOURCES})
target_link_libraries(comms_champion Qt5::Qml Qt5::Quick Qt5::Widgets)
