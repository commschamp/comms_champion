# Use this script to define cc::comms and cc::comms_champion targets
# when compiling CommsChampion using ExternalProject_Add() function.
# 
# Use "cc_define_external_project_targets (<future_install_dir> [NO_COMMS_CHAMPION] [QT_DIR <path_to_qt5>])
# macro. Usage of "NO_COMMS_CHAMPION" optional parameter will result in cc::comms_champion
# target not being created. The optional "QT_DIR" parameter may be used to specify 
# path to external Qt5 library (useful on windows platform).
#
# Output is provided using the following variables
#  CC_COMMS_FOUND - Found COMMS library, use CC_INCLUDE_DIRS for includes
#  CC_COMMS_CHAMPION_FOUND - Found CommsChampion, use CC_INCLUDE_DIRS, CC_PLUGIN_LIBRARIES, CC_PLUGIN_DIR)
#  CC_INCLUDE_DIRS - Where to find all the headers
#  CC_ROOT_DIR - Root directory of CommsChampion installation
#  CC_PLUGIN_LIBRARIES - Libraries (comms_champion) to link against when building a plugin
#  CC_PLUGIN_LIBRARY_DIRS - Where to find the libraries required to build plugin.
#  CC_PLUGIN_DIR - Directory where to install newly built plugin
#
# Additional output targets
# cc::comms - Link target for COMMS library
# cc::comms_champion - Link target for "comms_champion" libarary. Use it in plugin development.
#
# Deprecated inputs (when used will result in automatic "cc_define_external_project_targets()"
# macro invocation).
#   EXT_CC_INSTALL_DIR - Future CommsChampion install directory
#   EXT_CC_NO_COMMS_CHAMPION - Bool value to include comms_champion library target setup.
#   EXT_QT_DIR - Directory of Qt5 installation

macro (cc_define_external_project_targets inst_dir)
    set (_prefix CC_EXT_TGT)
    set (_options NO_COMMS_CHAMPION)
    set (_oneValueArgs QT_DIR)
    set (_mutiValueArgs)
    cmake_parse_arguments(${_prefix} "${_options}" "${_oneValueArgs}" "${_mutiValueArgs}" ${ARGN})

    set (CC_ROOT_DIR ${inst_dir})
    set (CC_COMMS_FOUND TRUE)
    set (CC_INCLUDE_DIRS "${inst_dir}/include")

    add_library(comms INTERFACE)
    add_library(cc::comms ALIAS comms)
    target_include_directories(comms INTERFACE ${CC_INCLUDE_DIRS})

    target_compile_options(comms INTERFACE
      $<$<CXX_COMPILER_ID:MSVC>:/wd4503 /wd4309 /wd4267 -D_SCL_SECURE_NO_WARNINGS>
    )
    
    while (TRUE)
        if (CC_EXT_TGT_NO_COMMS_CHAMPION)
            break ()
        endif ()

        set (cc_platform_specific)
        if (NOT "${CC_EXT_TGT_QT_DIR}" STREQUAL "")
            list (APPEND CMAKE_PREFIX_PATH ${CC_EXT_TGT_QT_DIR})
            
            if (WIN32)
                find_library(qt5platformsupport_rel qt5platformsupport HINTS "${EXT_QT_DIR}/lib")
                find_library(qt5platformsupport_deb qt5platformsupportd HINTS "${EXT_QT_DIR}/lib")

                if (qt5platformsupport_rel AND qt5platformsupport_deb)
                    add_library(qt5platformsupport_lib UNKNOWN IMPORTED)
                    set_target_properties(qt5platformsupport_lib PROPERTIES 
                        IMPORTED_LOCATION_DEBUG ${qt5platformsupport_rel})
                    set_target_properties(qt5platformsupport_lib PROPERTIES 
                        IMPORTED_LOCATION_RELEASE ${qt5platformsupport_deb})
                    list (APPEND cc_platform_specific qt5platformsupport_lib)
                else ()
                    message(WARNING "Platform support is missing!")
                endif ()
            endif ()   
        endif ()
        
        set (CC_COMMS_CHAMPION_FOUND TRUE)
        set (CC_PLUGIN_LIBRARIES "comms_champion")
        set (CC_PLUGIN_LIBRARY_DIRS ${inst_dir}/lib)
        set (CC_PLUGIN_DIR ${inst_dir}/lib/CommsChampion/plugin)

        if (WIN32)
            set (cc_lib_name ${CC_PLUGIN_LIBRARIES}.lib)
            list (APPEND cc_platform_specific Setupapi.lib Ws2_32.lib opengl32.lib imm32.lib winmm.lib)
        else ()
            set (cc_lib_name lib${CC_PLUGIN_LIBRARIES}.so)
        endif ()

        add_library(cc::comms_champion UNKNOWN IMPORTED)
        file (MAKE_DIRECTORY ${CC_INCLUDE_DIRS})
        set (interface_link_libs cc::comms)        

        find_package(Qt5Core)
        find_package(Qt5Widgets)

        if (Qt5Widgets_FOUND)
            list (APPEND interface_link_libs Qt5::Widgets)
        endif ()

        if (Qt5Core_FOUND)
            list (APPEND interface_link_libs Qt5::Core)
        endif ()

        if (cc_platform_specific)
            list (APPEND interface_link_libs ${cc_platform_specific})
        endif ()
        
        set_target_properties(cc::comms_champion PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES ${CC_INCLUDE_DIRS}
            IMPORTED_LOCATION ${CC_PLUGIN_LIBRARY_DIRS}/${cc_lib_name}
            INTERFACE_LINK_LIBRARIES "${interface_link_libs}"
        )
        
        break()
    endwhile ()
endmacro ()

# For backward compatibility, check the existence of EXT_CC_INSTALL_DIR definition
if (NOT "${EXT_CC_INSTALL_DIR}" STREQUAL "")
    set (cc_tgt_arg)
    if (EXT_CC_NO_COMMS_CHAMPION)
        set (cc_tgt_args NO_COMMS_CHAMPION)
    endif ()
    
    set (ext_qt_args)
    if (NOT "${EXT_QT_DIR}" STREQUAL "")
        set (ext_qt_args QT_DIR "${EXT_QT_DIR}")
    endif ()
    
    cc_define_external_project_targets("${EXT_CC_INSTALL_DIR}" ${cc_tgt_arg} ${ext_qt_args})
endif ()





