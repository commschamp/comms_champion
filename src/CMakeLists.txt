include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/comms/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/comms_champion/include
)

add_subdirectory (lib)

if (CC_COMMS_LIB_ONLY)
    return ()
endif ()

add_subdirectory (plugin)
add_subdirectory (app)
add_subdirectory (demo)
