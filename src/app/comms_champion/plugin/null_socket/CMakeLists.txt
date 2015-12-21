
function (plugin_dummy_socket)
    set (name "dummy_socket")
    
    set (meta_file "${CMAKE_CURRENT_SOURCE_DIR}/dummy_socket.json")
    set (stamp_file "${CMAKE_CURRENT_BINARY_DIR}/refresh_stamp.txt")
    if ((NOT EXISTS ${stamp_file}) OR (${meta_file} IS_NEWER_THAN ${stamp_file}))
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_SOURCE_DIR}/NullSocketPlugin.h)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E touch ${stamp_file})
    endif ()
    
    set (src
        NullSocket.cpp
        NullSocketPlugin.cpp
    )
    
    set (hdr
        NullSocket.h
        NullSocketPlugin.h
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

plugin_dummy_socket ()
