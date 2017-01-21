add_subdirectory (src)

if (UNIX)
    install(
        PROGRAMS script/comms_champion.sh
        DESTINATION ${BIN_INSTALL_DIR}
    )
endif ()  

if (WIN32 AND (NOT "${CC_QT_DIR}" STREQUAL ""))
    execute_process(
        COMMAND ${CMAKE_COMMAND} -DAPP_NAME=comms_champion -DEXTRA_PATH=${CC_QT_DIR}\\bin
            -DOUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
            -P ${CMAKE_SOURCE_DIR}/cmake/GenWinAppStartBat.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    
    install(
        PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/comms_champion.bat"
        DESTINATION ${BIN_INSTALL_DIR}
    )
endif ()