
function (plugin_echo_socket)
    set (name "echo_socket")
    
    if (NOT Qt5Core_FOUND)
        message(WARNING "Can NOT build ${name} due to missing Qt5Core library")
        return()
    endif ()
    
    set (meta_file "${CMAKE_CURRENT_SOURCE_DIR}/echo_socket.json")
    set (stamp_file "${CMAKE_CURRENT_BINARY_DIR}/refresh_stamp.txt")
    if ((NOT EXISTS ${stamp_file}) OR (${meta_file} IS_NEWER_THAN ${stamp_file}))
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_SOURCE_DIR}/EchoSocketPlugin.h)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E touch ${stamp_file})
    endif ()
    
    set (src
        EchoSocket.cpp
        EchoSocketPlugin.cpp
    )
    
    set (hdr
        EchoSocket.h
        EchoSocketPlugin.h
    )
    
    qt5_wrap_cpp(
        moc
        ${hdr}
    )
    
    add_library (${name} MODULE ${src} ${moc})
    qt5_use_modules(${name} Core)
    
    install (
        TARGETS ${name}
        DESTINATION ${PLUGIN_INSTALL_DIR})
    
endfunction()

######################################################################

find_package(Qt5Core)

plugin_echo_socket ()
