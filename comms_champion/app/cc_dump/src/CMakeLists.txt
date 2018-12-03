function (bin_cc_dump)
    set (name "cc_dump")
    
    if (NOT Qt5Core_FOUND)
        message(WARNING "Can NOT build ${name} due to missing Qt5Core library")
        return()
    endif ()
    
    set (src
        main.cpp
        AppMgr.cpp
        CsvDumpMessageHandler.cpp
        RecordMessageHandler.cpp
    )
    
    qt5_wrap_cpp(
        moc
        AppMgr.h
    )
    
    #qt5_add_resources(resources ${CMAKE_CURRENT_SOURCE_DIR}/ui.qrc)

    add_executable(${name} ${src} ${moc})
    target_link_libraries(${name} cc::comms_champion)
    qt5_use_modules(${name} Core)
    
    install (
        TARGETS ${name}
        DESTINATION ${BIN_INSTALL_DIR})
        
endfunction ()

###########################################################

find_package(Qt5Core)

include_directories (
#    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

bin_cc_dump()
