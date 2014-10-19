set (LOCAL_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set (TOP_HEADERS_DIR ${LOCAL_INC_DIR}/comms_champion)
set (HEADERS_DIR ${TOP_HEADERS_DIR}/protocol)
set (SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

######################################################################

function (lib_protocol)
    set (name ${PROTOCOL_LIB_NAME})
    set (src
        ${SOURCES_DIR}/MsgMgr.cpp
        ${SOURCES_DIR}/Message.cpp
        ${SOURCES_DIR}/HeartbeatMsg.cpp
    )
    
    qt5_wrap_cpp(
        moc
        ${HEADERS_DIR}/MsgMgr.h
        ${HEADERS_DIR}/Message.h            
        ${HEADERS_DIR}/Plugin.h
    )
    
    add_library (${name} SHARED ${src} ${moc})
    
    install (
        TARGETS ${name}
        DESTINATION ${LIB_INSTALL_DIR})
        
    install (
        DIRECTORY ${TOP_HEADERS_DIR}
        DESTINATION ${INC_INSTALL_DIR}
    )
    
endfunction()

######################################################################

include_directories(${Qt5Core_INCLUDE_DIRS})

lib_protocol()

