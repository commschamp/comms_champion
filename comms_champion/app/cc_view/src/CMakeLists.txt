function (bin_cc_view)
    set (name "cc_view")
    
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
        dir.cpp
        GuiAppMgr.cpp
        DefaultMessageDisplayHandler.cpp
        MsgFileMgrG.cpp
        PluginMgrG.cpp
        MsgMgrG.cpp
        icon.cpp
        widget/MainWindowWidget.cpp
        widget/MainToolbar.cpp
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
        widget/RawHexDataDialog.cpp
        widget/DefaultMessageWidget.cpp
        widget/ProtocolsStackWidget.cpp
        widget/MessageUpdateDialog.cpp
        widget/PluginsListWidget.cpp
        widget/PluginConfigDialog.cpp
        widget/field/FieldWidget.cpp
        widget/field/ShortIntValueFieldWidget.cpp
        widget/field/LongIntValueFieldWidget.cpp
        widget/field/LongLongIntValueFieldWidget.cpp
        widget/field/UnsignedLongLongIntValueFieldWidget.cpp
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
        widget/field/VariantFieldWidget.cpp
        widget/field/UnknownValueFieldWidget.cpp
    )
    
    qt5_wrap_cpp(
        moc
        GuiAppMgr.h
        widget/MessageDisplayWidget.h
        widget/MessageWidget.h
        widget/DefaultMessageWidget.h
        widget/RecvAreaToolBar.h
        widget/SendAreaToolBar.h
        widget/MsgListWidget.h
        widget/MsgDetailsWidget.h
        widget/ProtocolsStackWidget.h
        widget/MessageUpdateDialog.h
        widget/RawHexDataDialog.h
        widget/PluginsListWidget.h
        widget/PluginConfigDialog.h
        widget/MainWindowWidget.h
        widget/MainToolbar.h
        widget/DefaultMessageDisplayWidget.h
        widget/field/FieldWidget.h 
        widget/field/ShortIntValueFieldWidget.h
        widget/field/LongIntValueFieldWidget.h
        widget/field/LongLongIntValueFieldWidget.h
        widget/field/UnsignedLongLongIntValueFieldWidget.h
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
        widget/field/VariantFieldWidget.h
        widget/field/UnknownValueFieldWidget.h
    )
    
    qt5_wrap_ui(
        ui
        ui/MainWindowWidget.ui
        ui/MsgListWidget.ui
        ui/MsgDetailsWidget.ui
        ui/ProtocolsStackWidget.ui
        ui/MessageUpdateDialog.ui
        ui/RawHexDataDialog.ui
        ui/PluginsListWidget.ui
        ui/PluginConfigDialog.ui
        ui/ShortIntValueFieldWidget.ui
        ui/LongIntValueFieldWidget.ui
        ui/LongLongIntValueFieldWidget.ui
        ui/UnsignedLongLongIntValueFieldWidget.ui
        ui/BitmaskValueFieldWidget.ui
        ui/EnumValueFieldWidget.ui
        ui/StringFieldWidget.ui
        ui/BitfieldFieldWidget.ui
        ui/OptionalFieldWidget.ui
        ui/ArrayListRawDataFieldWidget.ui
        ui/ArrayListFieldWidget.ui
        ui/ArrayListElementWidget.ui
        ui/FloatValueFieldWidget.ui
        ui/VariantFieldWidget.ui
        ui/UnknownValueFieldWidget.ui
    )
    
    qt5_add_resources(resources ${CMAKE_CURRENT_SOURCE_DIR}/ui.qrc)

    add_executable(${name} ${src} ${moc} ${ui} ${resources})
    target_link_libraries(${name} cc::${COMMS_CHAMPION_LIB_NAME})
    qt5_use_modules(${name} Widgets Core)
    
    install (
        TARGETS ${name}
        DESTINATION ${BIN_INSTALL_DIR})
        
    install (
        FILES image/app_icon.png
        DESTINATION ${ICON_INSTALL_DIR}
        RENAME cc_view.png
    )
    
        
endfunction ()

###########################################################

find_package(Qt5Widgets)
find_package(Qt5Core)

include_directories (
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

bin_cc_view()
