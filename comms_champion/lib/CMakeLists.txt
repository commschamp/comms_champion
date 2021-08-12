add_subdirectory (src)

if (CC_INSTALL_TOOLS_LIBRARY)
    install (
        DIRECTORY "include/comms_champion"
        DESTINATION ${INC_INSTALL_DIR}
    )

    install (
        FILES ${COMMS_CHAMPION_CMAKE_FILES}
        DESTINATION ${LIB_INSTALL_DIR}/CommsChampion/cmake/
    )
endif()    

FILE(GLOB_RECURSE headers "include/*.h")
add_custom_target(comms_champion.headers SOURCES ${headers})
