function (bin_protocol_analyser)
    set (name "cc_protocol_analyser")

    set (src
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    )
    
    qt5_add_resources(resources ${CMAKE_CURRENT_SOURCE_DIR}/ui.qrc)

    add_executable(${name} ${src} ${resources})
    
    target_link_libraries(
        ${name}
        ${PROTOCOL_LIB_NAME} 
        Qt5::Qml Qt5::Quick Qt5::Widgets Qt5::Core
    )
endfunction ()

###########################################################

bin_protocol_analyser()
