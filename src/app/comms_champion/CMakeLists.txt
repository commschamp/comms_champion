function (bin_comms_champion)
    set (name "comms_champion")

    set (src
        src/main.cpp
        src/ErrorStatus.cpp
        src/MsgMgr.cpp
        src/Message.cpp
    )
    
    set (_inc_dir ${CMAKE_CURRENT_SOURCE_DIR}/include/comms_champion)
    qt5_wrap_cpp(
        moc
        ${_inc_dir}/MsgMgr.h
        ${_inc_dir}/Message.h            
    )
    
    qt5_add_resources(resources ${CMAKE_CURRENT_SOURCE_DIR}/ui.qrc)

    add_executable(${name} ${src} ${moc} ${resources})
    
    target_link_libraries(
        ${name}
        ${PROTOCOL_LIB_NAME} 
        Qt5::Qml Qt5::Quick Qt5::Widgets Qt5::Core
    )
    
    install (
        TARGETS ${name}
        DESTINATION ${BIN_INSTALL_DIR})
        
    install (
        DIRECTORY ${_inc_dir}
        DESTINATION ${INC_INSTALL_DIR}
    )
        
endfunction ()

###########################################################

bin_comms_champion()
