
function (plugin_udp_client_socket)
    set (name "udp_client_socket")
    
    if (NOT Qt5Core_FOUND)
        message(WARNING "Can NOT build ${name} due to missing Qt5Core library")
        return()
    endif ()
    
    if (NOT Qt5Widgets_FOUND)
        message(WARNING "Can NOT build ${name} due to missing Qt5Widgets library")
        return()
    endif ()
    
    if (NOT Qt5Network_FOUND)
        message(WARNING "Can NOT build ${name} due to missing Qt5Network library")
        return()
    endif ()
    
    set (meta_file "${CMAKE_CURRENT_SOURCE_DIR}/udp_client_socket.json")
    set (stamp_file "${CMAKE_CURRENT_BINARY_DIR}/client_refresh_stamp.txt")
    
    set (refresh_plugin_header TRUE)
    if ((NOT EXISTS ${stamp_file}) OR (${meta_file} IS_NEWER_THAN ${stamp_file}))
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_SOURCE_DIR}/SocketPlugin.h)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E touch ${stamp_file})
    endif ()
    
    set (src
        Plugin.cpp
        Socket.cpp
        SocketConfigWidget.cpp
    )
    
    set (hdr
        Plugin.h
        Socket.h
        SocketConfigWidget.h
    )
    
    qt5_wrap_cpp(
        moc
        ${hdr}
    )
    
    qt5_wrap_ui(
        ui
        SocketConfigWidget.ui
    )
    
    add_library (${name} MODULE ${src} ${moc} ${ui})
    target_link_libraries(${name} PRIVATE cc::${COMMS_CHAMPION_LIB_NAME} Qt5::Network Qt5::Widgets Qt5::Core)
    
    if (CC_INSTALL_TOOLS)
        install (
            TARGETS ${name}
            DESTINATION ${PLUGIN_INSTALL_DIR})
    endif ()
    
endfunction()

######################################################################

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Network)

include_directories (
    ${CMAKE_CURRENT_BINARY_DIR}
)

plugin_udp_client_socket ()
