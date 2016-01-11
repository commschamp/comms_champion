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
        DataInfo.cpp
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
    
    find_library(QWINDOWS_REL qwindows HINTS "${CC_QT_DIR}/plugins/platforms")
    find_library(QWINDOWS_DEB qwindowsd HINTS "${CC_QT_DIR}/plugins/platforms")
    
    set(QT5PLATFORMSUPPORT optimized ${QT5PLATFORMSUPPORT_REL} debug ${QT5PLATFORMSUPPORT_DEB})
    set(QWINDOWS optimized ${QWINDOWS_REL} debug ${QWINDOWS_DEB})
    
    set (CC_PLATFORM_SPECIFIC ${QT5PLATFORMSUPPORT} ${QWINDOWS} Setupapi.lib Ws2_32.lib opengl32.lib imm32.lib winmm.lib)

    if (MSVC14)
        find_library(QTPCRE_REL qtpcre HINTS "${CC_QT_DIR}/lib")
        find_library(QTPCRE_DEB qtpcred HINTS "${CC_QT_DIR}/lib")
        set(QTPCRE optimized ${QTPCRE_REL} debug ${QTPCRE_DEB})
        set (CC_PLATFORM_SPECIFIC ${QTPCRE} ${CC_PLATFORM_SPECIFIC})

        find_library(QTFREETYPE_REL qtfreetype HINTS "${CC_QT_DIR}/lib")
        find_library(QTFREETYPE_DEB qtfreetyped HINTS "${CC_QT_DIR}/lib")
        set(QTFREETYPE optimized ${QTFREETYPE_REL} debug ${QTFREETYPE_DEB})
        set (CC_PLATFORM_SPECIFIC ${QTFREETYPE} ${CC_PLATFORM_SPECIFIC})

        find_library(QTHARFBUZZING_REL qtharfbuzzng HINTS "${CC_QT_DIR}/lib")
        find_library(QTHARFBUZZING_DEB qtharfbuzzngd HINTS "${CC_QT_DIR}/lib")
        set(QTHARFBUZZING optimized ${QTHARFBUZZING_REL} debug ${QTHARFBUZZING_DEB})
        set (CC_PLATFORM_SPECIFIC ${QTHARFBUZZING} ${CC_PLATFORM_SPECIFIC})
    endif ()

endif ()   

lib_comms_champion()
