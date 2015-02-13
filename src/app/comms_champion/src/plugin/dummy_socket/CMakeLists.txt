
function (plugin_dummy_socket)
    set (name "dummy_socket")
    set (src
        DummySocket.cpp
        DummySocketPlugin.cpp
    )
    
    set (hdr
        DummySocket.h
        DummySocketPlugin.h
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
