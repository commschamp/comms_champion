if (NOT CC_BUILD_TOOLS_LIBRARY)
    return()
endif ()

set (CC_TOOLS_QT_EXTERNAL_PROJ_TGT "cc_tools_qt_external")
set (CC_TOOLS_QT_EXTERNAL_PROJ_REPO "https://github.com/commschamp/cc_tools_qt.git")
set (CC_TOOLS_QT_EXTERNAL_PROJ_TAG "v4.0")
set (CC_TOOLS_QT_EXTERNAL_PROJ_SRC_DIR "${CC_EXTERNALS_DIR}/cc_tools_qt")
set (CC_TOOLS_QT_EXTERNAL_PROJ_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/cc_tools_qt/build")
set (CC_TOOLS_QT_EXTERNAL_PROJ_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/cc_tools_qt/install")

if (CC_INSTALL_TOOLS_LIBRARY OR CC_INSTALL_TOOLS)
    set (CC_TOOLS_QT_EXTERNAL_PROJ_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")
endif ()

set (cxx_standard_opt)
if (NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
    set (cxx_standard_opt -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD})
endif ()

set (qt_opt)
if (NOT "${CC_QT_DIR}" STREQUAL "")
    set (qt_opt -DCC_TOOLS_QT_DIR=${CC_QT_DIR})
endif ()

list (APPEND CMAKE_PREFIX_PATH ${CC_COMMS_EXTERNAL_PROJ_INSTALL_DIR})

include(ExternalProject)
ExternalProject_Add(
    "${CC_TOOLS_QT_EXTERNAL_PROJ_TGT}"
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
    GIT_REPOSITORY "${CC_TOOLS_QT_EXTERNAL_PROJ_REPO}"
    GIT_TAG ${CC_TOOLS_QT_EXTERNAL_PROJ_TAG}
    SOURCE_DIR "${CC_TOOLS_QT_EXTERNAL_PROJ_SRC_DIR}"
    BINARY_DIR "${CC_TOOLS_QT_EXTERNAL_PROJ_BUILD_DIR}"
    INSTALL_DIR "${CC_TOOLS_QT_EXTERNAL_PROJ_INSTALL_DIR}"
    CMAKE_ARGS 
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} 
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CC_TOOLS_QT_EXTERNAL_PROJ_INSTALL_DIR}
        -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
        -DCC_TOOLS_QT_INSTALL_LIBRARY_HEADERS=${CC_INSTALL_TOOLS_LIBRARY}
        -DCC_TOOLS_QT_BUILD_APPS=${CC_BUILD_TOOLS}
        -DCC_TOOLS_QT_INSTALL_APPS=${CC_INSTALL_TOOLS}
        -DCC_TOOLS_QT_BUILD_PLUGINS=${CC_BUILD_TOOLS}
        -DCC_TOOLS_QT_INSTALL_PLUGINS=${CC_INSTALL_TOOLS}
        -DCC_TOOLS_QT_BUILD_DEMO_PROTOCOL=${CC_BUILD_DEMO_PROTOCOL}
        -DCC_TOOLS_QT_INSTALL_DEMO_PROTOCOL=${CC_INSTALL_DEMO_PROTOCOL}
        -DCC_TOOLS_QT_WARN_AS_ERR=${CC_WARN_AS_ERR}
        -DCC_TOOLS_QT_USE_CCACHE=${CC_USE_CCACHE}
        -DCC_TOOLS_QT_SKIP_CXX_STANDARD_FORCING=${CC_SKIP_CXX_STANDARD_FORCING}
        -DCC_TOOLS_QT_STATIC_RUNTIME=${CC_STATIC_RUNTIME}
        -DCC_TOOLS_QT_EXTERNAL_COMMS=ON
        -DCC_TOOLS_QT_EXTERNALS_DIR=${CC_EXTERNALS_DIR}
        ${cxx_standard_opt}
        ${qt_opt}
)

add_dependencies(${CC_TOOLS_QT_EXTERNAL_PROJ_TGT} cc::comms)

find_package (Doxygen)
if (DOXYGEN_FOUND)
    add_custom_target ("doc_commschampion"
        COMMAND ${CMAKE_COMMAND} 
            --build ${CC_TOOLS_QT_EXTERNAL_PROJ_BUILD_DIR} --target doc_cc_tools_qt
        DEPENDS ${CC_TOOLS_QT_EXTERNAL_PROJ_TGT}
    )
endif ()

if (WIN32 AND NOT ("${CC_QT_DIR}" STREQUAL ""))
    add_custom_target ("deploy_qt"
        COMMAND ${CMAKE_COMMAND} 
            --build ${CC_TOOLS_QT_EXTERNAL_PROJ_BUILD_DIR} --target deploy_qt
        DEPENDS ${CC_TOOLS_QT_EXTERNAL_PROJ_TGT}
    )
endif ()

