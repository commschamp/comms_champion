function (plugin_serial_socket)
    set (name "serial_socket")
    
    set (meta_file "${CMAKE_CURRENT_SOURCE_DIR}/serial_socket.json")
    set (stamp_file "${CMAKE_CURRENT_BINARY_DIR}/server_refresh_stamp.txt")
    
    set (refresh_plugin_header TRUE)
    if ((NOT EXISTS ${stamp_file}) OR (${meta_file} IS_NEWER_THAN ${stamp_file}))
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_SOURCE_DIR}/ServerSocketPlugin.h)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E touch ${stamp_file})
    endif ()
    
    set (src
        SerialSocket.cpp
        SerialSocketPlugin.cpp
        SerialSocketConfigWidget.cpp
    )
    
    set (hdr
        SerialSocket.h
        SerialSocketPlugin.h
        SerialSocketConfigWidget.h
    )
    
    qt5_wrap_cpp(
        moc
        ${hdr}
    )
    
    qt5_wrap_ui(
        ui
        SerialSocketConfigWidget.ui
    )
    
    add_library (${name} SHARED ${src} ${moc} ${ui})
    target_link_libraries(${name} Qt5::SerialPort Qt5::Core)
    
    install (
        TARGETS ${name}
        DESTINATION ${PLUGIN_INSTALL_DIR})
    
endfunction()

######################################################################

include_directories (
    ${Qt5Core_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
)

plugin_serial_socket ()
