
function (plugin_echo_socket)
    set (name "echo_socket")
    
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
    
    add_library (${name} SHARED ${src} ${moc})
    
    install (
        TARGETS ${name}
        DESTINATION ${PLUGIN_INSTALL_DIR})
    
endfunction()

######################################################################

include_directories(${Qt5Core_INCLUDE_DIRS})

plugin_echo_socket ()
