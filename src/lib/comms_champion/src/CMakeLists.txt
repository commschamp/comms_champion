function (lib_comms_champion)
    set (name "${COMMS_CHAMPION_LIB_TGT}")
    
    if (NOT Qt5Core_FOUND)
        message(WARNING "Can NOT build ${name} due to missing Qt5Core library")
        return()
    endif ()
    
    if (NOT Qt5Widgets_FOUND)
        message(WARNING "Can NOT build ${name} due to missing Qt5Widgets library")
        return()
    endif ()
    
    set (src
        ErrorStatus.cpp
        Message.cpp
        MessageInfo.cpp
        MessageHandler.cpp
        Property.cpp
        Plugin.cpp
        DataInfo.cpp
        PluginProperties.cpp
        ConfigMgr.cpp
        PluginMgr.cpp
        PluginMgrImpl.cpp
        MsgFileMgr.cpp
        MsgSendMgr.cpp
        MsgSendMgrImpl.cpp
        field_wrapper/FieldWrapper.cpp
        field_wrapper/IntValueWrapper.cpp
        field_wrapper/BitmaskValueWrapper.cpp
        field_wrapper/EnumValueWrapper.cpp
        field_wrapper/StringWrapper.cpp
        field_wrapper/BitfieldWrapper.cpp
        field_wrapper/OptionalWrapper.cpp
        field_wrapper/BundleWrapper.cpp
        field_wrapper/ArrayListRawDataWrapper.cpp
        field_wrapper/ArrayListWrapper.cpp
        field_wrapper/FloatValueWrapper.cpp
        field_wrapper/UnknownValueWrapper.cpp
    )
    
    qt5_wrap_cpp(
        moc
        MsgSendMgrImpl.h
    )
    
    add_library(${name} SHARED ${src} ${moc})
    qt5_use_modules(${name} Widgets Core)
    target_link_libraries(${name} ${CC_PLATFORM_SPECIFIC})
    
    set_target_properties(${name} PROPERTIES OUTPUT_NAME "${COMMS_CHAMPION_LIB_NAME}")
    
    install (
        TARGETS ${name}
        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
        RUNTIME DESTINATION ${BIN_INSTALL_DIR}
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
    )
        
        
endfunction ()

###########################################################

find_package(Qt5Core)
find_package(Qt5Widgets)

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_definitions (-DCC_LIB_EXPORT)
    
if (WIN32)
    find_library(QT5PLATFORMSUPPORT_REL Qt5PlatformSupport HINTS "${CC_QT_DIR}/lib")
    find_library(QT5PLATFORMSUPPORT_DEB Qt5PlatformSupportd HINTS "${CC_QT_DIR}/lib")
    set(QT5PLATFORMSUPPORT optimized ${QT5PLATFORMSUPPORT_REL} debug ${QT5PLATFORMSUPPORT_DEB})
    
    set (CC_PLATFORM_SPECIFIC ${QT5PLATFORMSUPPORT} Setupapi.lib Ws2_32.lib opengl32.lib imm32.lib winmm.lib)

endif ()   

lib_comms_champion()
