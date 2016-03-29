function (cc_plugin_demo)
    set (name "demo")
    
    set (meta_file "${CMAKE_CURRENT_SOURCE_DIR}/demo.json")
    set (stamp_file "${CMAKE_CURRENT_BINARY_DIR}/demo_refresh_stamp.txt")
    
    set (refresh_plugin_header TRUE)
    if ((NOT EXISTS ${stamp_file}) OR (${meta_file} IS_NEWER_THAN ${stamp_file}))
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_SOURCE_DIR}/Plugin.h)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E touch ${stamp_file})
    endif ()
    
    
    set (src
        Plugin.cpp
        Protocol.cpp
        TransportMessage.cpp
        message/IntValues.cpp
        message/EnumValues.cpp
        message/BitmaskValues.cpp
        message/Bitfields.cpp
    )
    
    set (hdr
        Plugin.h
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

find_package(Qt5Core REQUIRED)

include_directories(
    ${Qt5Core_INCLUDE_DIRS})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-backtrace-limit=0")
cc_plugin_demo ()
