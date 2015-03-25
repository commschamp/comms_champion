
function (plugin_demo)
    set (name "demo")
    set (src
        DemoPlugin.cpp
        Protocol.cpp
        message/CCDemoMessage.cpp
        message/CCTransportMessage.cpp
        message/CCRawDataMessage.cpp
        message/CCHeartbeat.cpp
        message/CCStatus.cpp
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

include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-backtrace-limit=0")
plugin_demo ()
