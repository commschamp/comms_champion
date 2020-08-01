# This script deploys Qt5 libraries on Windows.
# 
# Expected input variables
# CC_QT_DIR - Directory of Qt5 installation
# CC_BIN_DIR - Directory where all binaries are installed
# CC_PLUGIN_DIR - Directory where all plugins are installed

if (NOT WIN32)
    message (FATAL_ERROR "Qt5 deployment works only on Windows.")
endif ()

if ("${CC_QT_DIR}" STREQUAL "")
    message (FATAL_ERROR "Qt5 directory hasn't been provided.")
endif()

if ("${CC_BIN_DIR}" STREQUAL "")
    message (FATAL_ERROR "Directory of binaries hasn't been provided.")
endif()

if ("${CC_PLUGIN_DIR}" STREQUAL "")
    message (FATAL_ERROR "Directory of plugins hasn't been provided.")
endif()

find_program (deploy_exe windeployqt PATHS ${CC_QT_DIR}/bin)

if (NOT deploy_exe)
    message (FATAL_ERROR "windeployqt.exe hasn't been found.")
endif ()

file (GLOB binaries "${CC_BIN_DIR}/*.exe")
file (GLOB plugins "${CC_PLUGIN_DIR}/*.dll")

foreach (f ${binaries} ${plugins})
    execute_process (
        COMMAND ${deploy_exe} --dir ${CC_BIN_DIR} ${f}
    )
endforeach ()
