function (bin_comms_champion)
    set (name "comms_champion")

    set (src
        main.cpp
        ErrorStatus.cpp
        MsgMgr.cpp
        GuiAppMgr.cpp
        Message.cpp
        MessageInfo.cpp
        GlobalConstants.cpp
        Property.cpp
        DefaultMessageDisplayHandler.cpp
        FieldWidget.cpp
        ConfigMgr.cpp
        MsgFileMgr.cpp
        PluginControlInterface.cpp
        PluginControlInterfaceImpl.cpp
        PluginMgr.cpp
        Plugin.cpp
        icon.cpp
        widget/MainWindowWidget.cpp
        widget/LeftPaneWidget.cpp
        widget/RightPaneWidget.cpp
        widget/DefaultMessageDisplayWidget.cpp
        widget/RecvAreaToolBar.cpp
        widget/SendAreaToolBar.cpp
        widget/MsgListWidget.cpp
        widget/RecvMsgListWidget.cpp
        widget/SendMsgListWidget.cpp
        widget/MessageWidget.cpp
        widget/MsgDetailsWidget.cpp
        widget/DefaultMessageWidget.cpp
        widget/ProtocolsStackWidget.cpp
        widget/MessageUpdateDialog.cpp
        widget/PluginConfigDialog.cpp
        widget/field/BasicIntValueFieldWidget.cpp
        widget/field/BitmaskValueFieldWidget.cpp
        widget/field/BasicEnumValueFieldWidget.cpp
        widget/field/StringFieldWidget.cpp
        widget/field/BitfieldFieldWidget.cpp
        widget/field/UnknownValueFieldWidget.cpp
    )
    
    set (_inc_dir ${CMAKE_CURRENT_SOURCE_DIR}/../include/comms_champion)
    qt5_wrap_cpp(
        moc
        GuiAppMgr.h
        MsgMgr.h
        PluginMgr.h
        PluginControlInterfaceImpl.h
        widget/RecvAreaToolBar.h
        widget/SendAreaToolBar.h
        widget/MsgListWidget.h
        widget/MsgDetailsWidget.h
        widget/ProtocolsStackWidget.h
        widget/MessageUpdateDialog.h
        widget/PluginConfigDialog.h
        widget/MainWindowWidget.h
        widget/DefaultMessageDisplayWidget.h
        widget/field/BasicIntValueFieldWidget.h
        widget/field/BitmaskValueFieldWidget.h
        widget/field/BasicEnumValueFieldWidget.h
        widget/field/StringFieldWidget.h
        widget/field/BitfieldFieldWidget.h
        widget/field/UnknownValueFieldWidget.h
        ${_inc_dir}/Message.h      
        ${_inc_dir}/FieldWidget.h      
        ${_inc_dir}/MessageWidget.h
        ${_inc_dir}/DefaultMessageWidget.h
        ${_inc_dir}/MessageDisplayWidget.h
        ${_inc_dir}/Socket.h
    )
    
    qt5_wrap_ui(
        ui
        ui/MainWindowWidget.ui
        ui/MsgListWidget.ui
        ui/MsgDetailsWidget.ui
        ui/ProtocolsStackWidget.ui
        ui/MessageUpdateDialog.ui
        ui/PluginConfigDialog.ui
        ui/BasicIntValueFieldWidget.ui
        ui/BitmaskValueFieldWidget.ui
        ui/BasicEnumValueFieldWidget.ui
        ui/StringFieldWidget.ui
        ui/BitfieldFieldWidget.ui
        ui/UnknownValueFieldWidget.ui
    )
    
    qt5_add_resources(resources ${CMAKE_CURRENT_SOURCE_DIR}/ui.qrc)

    add_executable(${name} ${src} ${moc} ${ui} ${resources})
    
    target_link_libraries(
        ${name}
        ${PROTOCOL_LIB_NAME} 
        Qt5::Widgets Qt5::Core
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
    ${Qt5Core_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

bin_comms_champion()
