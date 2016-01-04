
function (plugin_demo)
    set (name "demo")
    
    if (NOT Qt5Core_FOUND)
        message(WARNING "Can NOT build ${name} due to missing Qt5Core library")
        return()
    endif ()
    
    if (NOT Qt5Widgets_FOUND)
        message(WARNING "Can NOT build ${name} due to missing Qt5Widgets library")
        return()
    endif ()

    set (meta_file "${CMAKE_CURRENT_SOURCE_DIR}/demo_protocol.json")
    set (stamp_file "${CMAKE_CURRENT_BINARY_DIR}/refresh_stamp.txt")
    if ((NOT EXISTS ${stamp_file}) OR (${meta_file} IS_NEWER_THAN ${stamp_file}))
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_SOURCE_DIR}/DemoPlugin.h)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E touch ${stamp_file})
    endif ()
    
    set (src
        DemoPlugin.cpp
        Protocol.cpp
        message/CCDemoMessage.cpp
        message/CCTransportMessage.cpp
        message/CCRawDataMessage.cpp
        message/CCHeartbeat.cpp
        message/CCStatus.cpp
        message/CCSerialInfo.cpp
        message/CCOptionalTest.cpp
    )
    
    set (hdr
        DemoPlugin.h
    )
    
    
    qt5_wrap_cpp(
        moc
        ${hdr}
    )
    
    add_library (${name} SHARED ${src} ${moc})
    qt5_use_modules(${name} Widgets Core)
    
    install (
        TARGETS ${name}
        DESTINATION ${PLUGIN_INSTALL_DIR})
    
endfunction()

######################################################################

find_package(Qt5Widgets)
find_package(Qt5Core)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-backtrace-limit=0")
plugin_demo ()
