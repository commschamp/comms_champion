function (lib_comms_champion)
    set (name ${COMMS_CHAMPION_LIB_NAME})
    set (src
        ${CMAKE_CURRENT_SOURCE_DIR}/MsgMgr.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Message.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/HeartbeatMsg.cpp
    )
    
    qt_wrap_cpp(
        ${name}
        src
        ${LOCAL_INC_DIR}/MsgMgr.h
        ${LOCAL_INC_DIR}/Message.h            
    )
    
    add_library (${name} SHARED ${src})
    
endfunction()

######################################################################

include_directories(${Qt5Core_INCLUDE_DIRS})

lib_comms_champion()
