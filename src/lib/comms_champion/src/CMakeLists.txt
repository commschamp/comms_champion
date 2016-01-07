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
        FieldWrapper.cpp
        Plugin.cpp
        PluginControlInterface.cpp
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
    
    add_library(${name} SHARED ${src})
    qt5_use_modules(${name} Widgets Core)
    
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

lib_comms_champion()