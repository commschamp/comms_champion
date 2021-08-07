add_subdirectory (src)

if (UNIX AND CC_INSTALL_TOOLS)
    install(
        PROGRAMS script/cc_dump.sh
        DESTINATION ${BIN_INSTALL_DIR}
    )
endif ()  

if (WIN32 AND (NOT "${CC_QT_DIR}" STREQUAL "") AND CC_INSTALL_TOOLS)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -DAPP_NAME=cc_dump -DEXTRA_PATH=${CC_QT_DIR}\\bin
            -DOUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
            -P ${PROJECT_SOURCE_DIR}/cmake/CC_GenWinAppStartBat.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    
    install(
        PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/cc_dump.bat"
        DESTINATION ${BIN_INSTALL_DIR}
    )
endif ()
