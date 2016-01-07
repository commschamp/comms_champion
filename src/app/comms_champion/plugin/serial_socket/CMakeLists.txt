function (plugin_serial_socket)
    set (name "serial_socket")
    
    if (NOT Qt5Core_FOUND)
        message(WARNING "Can NOT build ${name} due to missing Qt5Core library")
        return()
    endif ()
    
    if (NOT Qt5Widgets_FOUND)
        message(WARNING "Can NOT build ${name} due to missing Qt5Widgets library")
        return()
    endif ()
    
    if (NOT Qt5SerialPort_FOUND)
        message(WARNING "Can NOT build ${name} due to missing Qt5SerialPort library")
        return()
    endif ()
    
    
    set (meta_file "${CMAKE_CURRENT_SOURCE_DIR}/serial_socket.json")
    set (stamp_file "${CMAKE_CURRENT_BINARY_DIR}/server_refresh_stamp.txt")
    
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
    
    add_library (${name} MODULE ${src} ${moc} ${ui})
    qt5_use_modules(${name} SerialPort Widgets Core)
    
    install (
        TARGETS ${name}
        DESTINATION ${PLUGIN_INSTALL_DIR})
    
endfunction()

######################################################################

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5SerialPort)

include_directories (
    ${CMAKE_CURRENT_BINARY_DIR}
)

plugin_serial_socket ()
