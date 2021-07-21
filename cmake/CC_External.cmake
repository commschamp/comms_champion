# This file contains several helper functions to allow easy 
# inclusion of the CommsChampion project in other cmake project.

# Available functions  are:
#
# ******************************************************
# - Pull comms_champion sources.
#     cc_pull_sources(
#         SRC_DIR <src_dir>
#         [TAG <tag>]
#         [REPO <repo>]
#     )
# - SRC_DIR - A directory where comms_champion sources will end up.
# - TAG - Override the default tag to checkout.
# - REPO - Override the default repository of the comms_champion.
#
# ******************************************************
# - Build comms_champion during cmake configuration stage. Use it 
#   when the compilation process is light, i.e. only comms library is installed.
#     cc_build_during_config(
#         SRC_DIR <src_dir>
#         [BUILD_dir <build_dir>]
#         [TAG <tag>]
#         [REPO <repo>]
#         [CMAKE_ARGS <arg1> <arg2> ...]
#         [NO_DEFAULT_CMAKE_ARGS]
#     )
# - SRC_DIR - A directory where comms_champion sources will end up.
# - BUILD_DIR - A build directory, defaults to ${PROJECT_BINARY_DIR}/comms_champion
# - TAG - Override the default tag to checkout.
# - REPO - Override the default repository of the comms_champion.
# - CMAKE_ARGS - Extra cmake arguments to be passed to the comms_champion project.
#       Use CMAKE_INSTALL_PREFIX to specify where the output needs to be installed.
# - NO_DEFAULT_CMAKE_ARGS - Exclude passing the extra cmake arguments that copy the 
#       following values from the invoking project:
#         * CMAKE_C_COMPILER
#         * CMAKE_CXX_COMPILER
#         * CMAKE_TOOLCHAIN_FILE
#         * CMAKE_GENERATOR
#         * CMAKE_BUILD_TYPE
#         * CMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
#       The default values are passed before ones specified in CMAKE_ARGS, which can overwrite 
#       some of the default values.
#
# ******************************************************
# - Build comms_champion as external project during normal build process.
#     cc_build_as_external_project(
#         SRC_DIR <src_dir>
#         [BUILD_DIR <build_dir>]
#         [INSTALL_DIR] <install_dir>]
#         [TAG <tag>]
#         [REPO <repo>]
#         [CMAKE_ARGS <arg1> <arg2> ...]
#         [QT_DIR <path_to_qt>]
#         [NO_REPO]
#         [NO_TOOLS]
#         [UPDATE_DISCONNECTED]
#         [NO_DEFAULT_CMAKE_ARGS]
#     )
# - SRC_DIR - A directory where comms_champion sources will end up.
# - BUILD_DIR - A directory where comms_champion will be build.
# - INSTALL_DIR - A directory where comms_champion will be installed, also passed as 
#       CMAKE_INSTALL_PREFIX in addition to provided CMAKE_ARGS.
# - TAG - Override the default tag to checkout (unless NO_REPO param is used).
# - REPO - Override the default repository of the comms_champion (unless NO_REPO param is used).
# - CMAKE_ARGS - Extra cmake arguments to be passed to the comms_champion project.
# - QT_DIR - Path to external Qt5 libraries, will be passed to the comms_champion 
#   build process using CC_QT_DIR variable.
# - NO_REPO - Don't checkout sources, SRC_DIR must contain checkout out sources, suitable for this repo being a submodule.
# - NO_TOOLS - Will disable build of the CommsChampion Tools, will result in 
#   having CC_COMMS_LIB_ONLY=ON option being passed to the build process.
# - NO_DEPLOY_QT - Don't generate "deploy_qt" build target when applicable.
# - UPDATE_DISCONNECTED - Pass "UPDATE_DISCONNECTED 1" to ExternalProject_Add()
# - NO_DEFAULT_CMAKE_ARGS - Exclude passing the extra cmake arguments that copy the 
#       following values from the invoking project:
#         * CMAKE_C_COMPILER
#         * CMAKE_CXX_COMPILER
#         * CMAKE_TOOLCHAIN_FILE
#         * CMAKE_GENERATOR
#         * CMAKE_BUILD_TYPE
#         * CMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
#       The default values are passed before ones specified in CMAKE_ARGS, which can overwrite 
#       some of the copied values.
#
# ******************************************************

set (CC_EXTERNAL_DEFAULT_REPO "https://github.com/commschamp/comms_champion.git")
set (CC_EXTERNAL_DEFAULT_TAG "master")

function (cc_pull_sources)
    set (_prefix CC_PULL)
    set (_options)
    set (_oneValueArgs SRC_DIR REPO TAG)
    set (_mutiValueArgs)
    cmake_parse_arguments(${_prefix} "${_options}" "${_oneValueArgs}" "${_mutiValueArgs}" ${ARGN})

    if (NOT CC_PULL_SRC_DIR)
        message (FATAL_ERROR "The SRC_DIR parameter is not provided")
    endif ()     

    if (NOT CC_PULL_REPO)
        set (CC_PULL_REPO ${CC_EXTERNAL_DEFAULT_REPO})
    endif ()  

    if (NOT CC_PULL_TAG)
        set (CC_PULL_TAG ${CC_EXTERNAL_DEFAULT_TAG})
    endif ()  

    if (NOT GIT_FOUND)
        find_package(Git REQUIRED)
    endif ()

    if (EXISTS "${CC_PULL_SRC_DIR}/.git")
        message (STATUS "Updating existing comms_champion repository")
        execute_process (
            COMMAND ${GIT_EXECUTABLE} pull 
            WORKING_DIRECTORY ${CC_PULL_SRC_DIR}
        )
    
        execute_process (
            COMMAND ${GIT_EXECUTABLE} checkout ${CC_PULL_TAG}
            WORKING_DIRECTORY ${CC_PULL_SRC_DIR}
            RESULT_VARIABLE git_result
        )
        
        if (NOT "${git_result}" STREQUAL "0")
            message (WARNING "git checkout failed")
        endif ()

        return()
    endif()

    message (STATUS "Cloning comms_champion repository from scratch")

    execute_process (
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CC_PULL_SRC_DIR}"
    )
    
    execute_process (
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CC_PULL_SRC_DIR}"
    )        
    
    execute_process (
        COMMAND 
            ${GIT_EXECUTABLE} clone -b ${CC_PULL_TAG} ${CC_PULL_REPO} ${CC_PULL_SRC_DIR}
        RESULT_VARIABLE git_result
    )

    if (NOT "${git_result}" STREQUAL "0")
        message (WARNING "git clone/checkout failed")
    endif ()
endfunction ()

function (cc_build_during_config)
    set (_prefix CC_BUILD)
    set (_options NO_DEFAULT_CMAKE_ARGS)
    set (_oneValueArgs SRC_DIR BUILD_DIR REPO TAG)
    set (_mutiValueArgs CMAKE_ARGS)
    cmake_parse_arguments(${_prefix} "${_options}" "${_oneValueArgs}" "${_mutiValueArgs}" ${ARGN})

    if (NOT CC_BUILD_SRC_DIR)
        message (FATAL_ERROR "The SRC_DIR parameter is not provided")
    endif () 

    if (NOT CC_BUILD_BUILD_DIR)
        set (CC_BUILD_BUILD_DIR ${PROJECT_BINARY_DIR}/comms_champion)
    endif ()      

    if (NOT CC_BUILD_REPO)
        set (CC_BUILD_REPO ${CC_EXTERNAL_DEFAULT_REPO})
    endif ()  

    if (NOT CC_BUILD_TAG)
        set (CC_BUILD_TAG ${CC_EXTERNAL_DEFAULT_TAG})
    endif () 

    cc_pull_sources(SRC_DIR ${CC_BUILD_SRC_DIR} REPO ${CC_BUILD_REPO} TAG ${CC_BUILD_TAG})      

    execute_process (
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CC_BUILD_BUILD_DIR}"
    )

    set (extra_cmake_args)
    if (NOT CC_BUILD_NO_DEFAULT_CMAKE_ARGS)
        set (extra_cmake_args
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} 
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
            -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        )
    endif ()

    execute_process(
        COMMAND ${CMAKE_COMMAND} ${CC_BUILD_SRC_DIR}
            ${extra_cmake_args} ${CC_BUILD_CMAKE_ARGS}
        WORKING_DIRECTORY ${CC_BUILD_BUILD_DIR}
        RESULT_VARIABLE cc_cmake_result
    )

    if (NOT ${cc_cmake_result} EQUAL 0)
        message (FATAL_ERROR "CMake run for comms_champion has failed with result ${cc_cmake_result}")
    endif ()

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ${CC_BUILD_BUILD_DIR} --target install
        WORKING_DIRECTORY ${CC_BUILD_BUILD_DIR}
        RESULT_VARIABLE cc_build_result
    )

    if (NOT ${cc_build_result} EQUAL 0)
        message (FATAL_ERROR "Build of comms_champion has failed with result ${cc_build_result}")
    endif ()    
endfunction ()

macro (cc_define_external_project_targets inst_dir)
    set (_prefix CC_EXT_TGT)
    set (_options NO_COMMS_CHAMPION NO_TOOLS)
    set (_oneValueArgs QT_DIR)
    set (_mutiValueArgs)
    cmake_parse_arguments(${_prefix} "${_options}" "${_oneValueArgs}" "${_mutiValueArgs}" ${ARGN})

    set (CC_ROOT_DIR ${inst_dir})
    set (CC_COMMS_FOUND TRUE)
    set (CC_INCLUDE_DIRS "${inst_dir}/include")

    add_library(cc::comms INTERFACE IMPORTED GLOBAL)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CC_INCLUDE_DIRS}
    )    
    target_include_directories(cc::comms INTERFACE ${CC_INCLUDE_DIRS})

    while (TRUE)
        if (CC_EXT_TGT_NO_COMMS_CHAMPION OR CC_EXT_TGT_NO_TOOLS)
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

        add_library(cc::comms_champion UNKNOWN IMPORTED GLOBAL)
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

function (cc_build_as_external_project)
    set (_prefix CC_EXTERNAL_PROJ)
    set (_options NO_TOOLS NO_DEPLOY_QT UPDATE_DISCONNECTED NO_DEFAULT_CMAKE_ARGS NO_REPO)
    set (_oneValueArgs SRC_DIR BUILD_DIR INSTALL_DIR REPO TAG QT_DIR TGT)
    set (_mutiValueArgs CMAKE_ARGS)
    cmake_parse_arguments(${_prefix} "${_options}" "${_oneValueArgs}" "${_mutiValueArgs}" ${ARGN})

    if (NOT CC_EXTERNAL_PROJ_SRC_DIR)
        message (FATAL_ERROR "The SRC_DIR parameter is not provided")
    endif () 

    if (NOT CC_EXTERNAL_PROJ_BUILD_DIR)
        set (CC_EXTERNAL_PROJ_BUILD_DIR ${PROJECT_BINARY_DIR}/comms_champion)
    endif ()   

    if (NOT CC_EXTERNAL_PROJ_INSTALL_DIR)
        set (CC_EXTERNAL_PROJ_INSTALL_DIR ${CC_EXTERNAL_PROJ_BUILD_DIR}/install)
    endif ()  

    set (repo_param)
    set (repo_tag_param)
    if (NOT CC_EXTERNAL_PROJ_NO_REPO)
        if (NOT CC_EXTERNAL_PROJ_REPO)
            set (CC_EXTERNAL_PROJ_REPO ${CC_EXTERNAL_DEFAULT_REPO})
        endif ()  

        if (NOT CC_EXTERNAL_PROJ_TAG)
            set (CC_EXTERNAL_PROJ_TAG ${CC_EXTERNAL_DEFAULT_TAG})
        endif ()            


        set (repo_param GIT_REPOSITORY "${CC_EXTERNAL_PROJ_REPO}")
        set (repo_tag_param GIT_TAG "${CC_EXTERNAL_PROJ_TAG}")
    endif ()

    set (exteral_proj_qt_dir_param)
    if (CC_EXTERNAL_PROJ_QT_DIR)
        set (exteral_proj_qt_dir_param "-DCC_QT_DIR=${CC_EXTERNAL_PROJ_QT_DIR}")
    endif()

    set (comms_lib_only_param)
    if (CC_EXTERNAL_PROJ_NO_TOOLS)
        set (comms_lib_only_param "-DCC_COMMS_LIB_ONLY=ON")
    endif ()

    if (NOT CC_EXTERNAL_PROJ_TGT)
        set (CC_EXTERNAL_PROJ_TGT "cc_external_proj_tgt")
    endif ()

    set (cc_update_disconnected_opt)
    if (CC_EXTERNAL_PROJ_UPDATE_DISCONNECTED)
        set (cc_update_disconnected_opt "UPDATE_DISCONNECTED 1")
    endif ()    

    set (extra_cmake_args)
    if (NOT CC_EXTERNAL_PROJ_NO_DEFAULT_CMAKE_ARGS)
        set (extra_cmake_args
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} 
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
            -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        )
    endif ()

    include(ExternalProject)
    ExternalProject_Add(
        "${CC_EXTERNAL_PROJ_TGT}"
        PREFIX "${CC_EXTERNAL_PROJ_BUILD_DIR}"
        ${repo_param}
        ${repo_tag_param}
        SOURCE_DIR "${CC_EXTERNAL_PROJ_SRC_DIR}"
        BINARY_DIR "${CC_EXTERNAL_PROJ_BUILD_DIR}"
        INSTALL_DIR "${CC_EXTERNAL_PROJ_INSTALL_DIR}"
        ${cc_update_disconnected_opt}
        CMAKE_ARGS 
            ${extra_cmake_args}
            ${exteral_proj_qt_dir_param} ${comms_lib_only_param}
            -DCMAKE_INSTALL_PREFIX=${CC_EXTERNAL_PROJ_INSTALL_DIR}
            ${CC_EXTERNAL_PROJ_CMAKE_ARGS}
    )    

    set (define_targets_qt_dir_param)
    if (CC_EXTERNAL_PROJ_QT_DIR)
        set (define_targets_qt_dir_param "QT_DIR ${CC_EXTERNAL_PROJ_QT_DIR}")
    endif()    

    set (define_targets_no_tools_param)
    if (CC_EXTERNAL_PROJ_NO_TOOLS)
        set (define_targets_no_tools_param "NO_TOOLS")
    endif() 

    cc_define_external_project_targets(
        ${CC_EXTERNAL_PROJ_INSTALL_DIR} 
        ${define_targets_qt_dir_param}
        ${define_targets_no_tools_param}
    )

    if (TARGET cc::comms)
        add_dependencies(cc::comms ${CC_EXTERNAL_PROJ_TGT})
    endif ()

    if (TARGET cc::comms_champion)
        add_dependencies(cc::comms_champion ${CC_EXTERNAL_PROJ_TGT})
    endif ()

    if ((NOT CC_EXTERNAL_PROJ_NO_DEPLOY_QT) AND WIN32 AND (NOT "${CC_EXTERNAL_PROJ_QT_DIR}" STREQUAL ""))
        message (STATUS "Qt5 deployment is available by building \"deploy_qt\" target")
        add_custom_target ("deploy_qt"
            COMMAND ${CMAKE_COMMAND} --build ${CC_EXTERNAL_PROJ_BUILD_DIR} --target deploy_qt
            WORKING_DIRECTORY ${CC_EXTERNAL_PROJ_BUILD_DIR}
        )

        add_dependencies("deploy_qt" ${CC_EXTERNAL_PROJ_TGT})
    endif ()

endfunction ()
