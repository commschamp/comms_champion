function (bin_comms_champion)
    set (name "comms_champion")
    
    if (NOT Qt5Core_FOUND)
        message(WARNING "Can NOT build ${name} due to missing Qt5Core library")
        return()
    endif ()
    
    if (NOT Qt5Widgets_FOUND)
        message(WARNING "Can NOT build ${name} due to missing Qt5Widgets library")
        return()
    endif ()

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
        FieldWidgetCreator.cpp
        FieldWidget.cpp
        FieldWrapper.cpp
        ConfigMgr.cpp
        MsgFileMgr.cpp
        PluginControlInterface.cpp
        PluginControlInterfaceImpl.cpp
        PluginControlInterfaceSocket.cpp
        PluginControlInterfaceProtocol.cpp
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
        widget/PluginsListWidget.cpp
        widget/PluginConfigDialog.cpp
        widget/field/ShortIntValueFieldWidget.cpp
        widget/field/LongIntValueFieldWidget.cpp
        widget/field/ScaledIntValueFieldWidget.cpp
        widget/field/IntValueFieldWidget.cpp
        widget/field/BitmaskValueFieldWidget.cpp
        widget/field/EnumValueFieldWidget.cpp
        widget/field/StringFieldWidget.cpp
        widget/field/BitfieldFieldWidget.cpp
        widget/field/OptionalFieldWidget.cpp
        widget/field/BundleFieldWidget.cpp
        widget/field/ArrayListRawDataFieldWidget.cpp
        widget/field/ArrayListFieldWidget.cpp
        widget/field/FloatValueFieldWidget.cpp
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
        widget/PluginsListWidget.h
        widget/PluginConfigDialog.h
        widget/MainWindowWidget.h
        widget/DefaultMessageDisplayWidget.h
        widget/field/ShortIntValueFieldWidget.h
        widget/field/LongIntValueFieldWidget.h
        widget/field/ScaledIntValueFieldWidget.h
        widget/field/BitmaskValueFieldWidget.h
        widget/field/EnumValueFieldWidget.h
        widget/field/StringFieldWidget.h
        widget/field/BitfieldFieldWidget.h
        widget/field/OptionalFieldWidget.h
        widget/field/BundleFieldWidget.h
        widget/field/ArrayListRawDataFieldWidget.h
        widget/field/ArrayListFieldWidget.h
        widget/field/FloatValueFieldWidget.h
        widget/field/UnknownValueFieldWidget.h
        ${_inc_dir}/Message.h      
        ${_inc_dir}/FieldWidget.h      
        ${_inc_dir}/MessageWidget.h
        ${_inc_dir}/DefaultMessageWidget.h
        ${_inc_dir}/MessageDisplayWidget.h
    )
    
    qt5_wrap_ui(
        ui
        ui/MainWindowWidget.ui
        ui/MsgListWidget.ui
        ui/MsgDetailsWidget.ui
        ui/ProtocolsStackWidget.ui
        ui/MessageUpdateDialog.ui
        ui/PluginsListWidget.ui
        ui/PluginConfigDialog.ui
        ui/ShortIntValueFieldWidget.ui
        ui/LongIntValueFieldWidget.ui
        ui/BitmaskValueFieldWidget.ui
        ui/EnumValueFieldWidget.ui
        ui/StringFieldWidget.ui
        ui/BitfieldFieldWidget.ui
        ui/OptionalFieldWidget.ui
        ui/ArrayListRawDataFieldWidget.ui
        ui/ArrayListFieldWidget.ui
        ui/ArrayListElementWidget.ui
        ui/FloatValueFieldWidget.ui
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
    
    install (
        FILES image/app_icon.png
        DESTINATION ${ICON_INSTALL_DIR}
        RENAME comms_champion.png
    )
    
        
endfunction ()

###########################################################

find_package(Qt5Widgets)
find_package(Qt5Core)

include_directories (
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

bin_comms_champion()
