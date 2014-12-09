function (bin_comms_champion)
    set (name "comms_champion")

    set (src
        src/main.cpp
        src/ErrorStatus.cpp
        src/MsgMgr.cpp
        src/GuiAppMgr.cpp
        src/Message.cpp
        src/MessageInfo.cpp
        src/GlobalConstants.cpp
        src/WidgetProxy.cpp
        src/DefaultMessageDisplayHandler.cpp
        src/FieldWidget.cpp
        src/widget/MainWindowWidget.cpp
        src/widget/LeftPaneWidget.cpp
        src/widget/RightPaneWidget.cpp
        src/widget/RecvAreaToolBar.cpp
        src/widget/SendAreaToolBar.cpp
        src/widget/MsgListWidget.cpp
        src/widget/RecvMsgListWidget.cpp
        src/widget/SendMsgListWidget.cpp
        src/widget/MessageWidget.cpp
        src/widget/MsgDetailsWidget.cpp
        src/widget/DefaultMessageWidget.cpp
        src/widget/field/BasicIntValueFieldWidget.cpp
        src/widget/field/BitmaskValueFieldWidget.cpp
        src/widget/field/BasicEnumValueFieldWidget.cpp
        src/widget/field/UnknownValueFieldWidget.cpp
    )
    
    set (_inc_dir ${CMAKE_CURRENT_SOURCE_DIR}/include/comms_champion)
    qt5_wrap_cpp(
        moc
        src/GuiAppMgr.h
        src/MsgMgr.h
        src/GlobalConstants.h
        src/WidgetProxy.h
        src/widget/RecvAreaToolBar.h
        src/widget/MsgListWidget.h
        src/widget/MsgDetailsWidget.h
        src/widget/field/BasicIntValueFieldWidget.h
        src/widget/field/BitmaskValueFieldWidget.h
        src/widget/field/BasicEnumValueFieldWidget.h
        src/widget/field/UnknownValueFieldWidget.h
        ${_inc_dir}/Message.h      
        ${_inc_dir}/FieldWidget.h      
        ${_inc_dir}/MessageWidget.h
        ${_inc_dir}/DefaultMessageWidget.h
    )
    
    qt5_wrap_ui(
        ui
        ui/MainWindowWidget.ui
        ui/MsgListWidget.ui
        ui/MsgDetailsWidget.ui
        ui/BasicIntValueFieldWidget.ui
        ui/BitmaskValueFieldWidget.ui
        ui/BasicEnumValueFieldWidget.ui
        ui/UnknownValueFieldWidget.ui
    )
    
    qt5_add_resources(resources ${CMAKE_CURRENT_SOURCE_DIR}/ui.qrc)

    add_executable(${name} ${src} ${moc} ${ui} ${resources})
    
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

include_directories (
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

bin_comms_champion()
