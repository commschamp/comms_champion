
function (plugin_demo)
    set (name "demo")
    set (src
        DemoPlugin.cpp
    )
    
    set (hdr
        DemoPlugin.h
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

plugin_demo ()
