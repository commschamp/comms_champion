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
#         [TAG <tag>]
#         [REPO <repo>]
#         [CMAKE_ARGS <arg1> <arg2> ...]
#     )
# - SRC_DIR - A directory where comms_champion sources will end up.
# - TAG - Override the default tag to checkout.
# - REPO - Override the default repository of the comms_champion.
# - CMAKE_ARGS - Extra cmake arguments to be passed to the comms_champion project.
#   Use CMAKE_INSTALL_PREFIX to specify where the output needs to be installed.
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
#         [NO_TOOLS]
#     )
# - SRC_DIR - A directory where comms_champion sources will end up.
# - BUILD_DIR - A directory where comms_champion will be build.
# - INSTALL_DIR - A directory where comms_champion will be installed.
# - TAG - Override the default tag to checkout.
# - REPO - Override the default repository of the comms_champion.
# - CMAKE_ARGS - Extra cmake arguments to be passed to the comms_champion project.
# - QT_DIR - Path to external Qt5 libraries, will be passed to the comms_champion 
#   build process using CC_QT_DIR variable.
# - NO_TOOLS - Will disable build of the CommsChampion Tools, will result in 
#   having CC_COMMS_LIB_ONLY=ON option being passed to the build process.
#
# ******************************************************

set (CC_EXTERNAL_DEFAULT_REPO "https://github.com/arobenko/comms_champion.git")
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

    execute_process (
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CC_PULL_SRC_DIR}"
    )
    
    execute_process (
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CC_PULL_SRC_DIR}"
    )        
    
    execute_process (
        COMMAND 
            ${GIT_EXECUTABLE} clone -b ${CC_PULL_TAG} --depth 1 ${CC_PULL_REPO} ${CC_PULL_SRC_DIR}
        RESULT_VARIABLE git_result
    )

    if (NOT "${git_result}" STREQUAL "0")
        message (WARNING "git clone/checkout failed")
    endif ()
endfunction ()

function (cc_build_during_config)
    set (_prefix CC_BUILD)
    set (_options)
    set (_oneValueArgs SRC_DIR BUILD_DIR REPO TAG)
    set (_mutiValueArgs CMAKE_ARGS)
    cmake_parse_arguments(${_prefix} "${_options}" "${_oneValueArgs}" "${_mutiValueArgs}" ${ARGN})

    if (NOT CC_BUILD_SRC_DIR)
        message (FATAL_ERROR "The SRC_DIR parameter is not provided")
    endif () 

    if (NOT CC_BUILD_BUILD_DIR)
        set (CC_BUILD_BUILD_DIR ${CC_BUILD_SRC_DIR}/build)
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

    execute_process(
        COMMAND ${CMAKE_COMMAND} ${CC_BUILD_SRC_DIR}
            ${CC_BUILD_CMAKE_ARGS}
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

function (cc_build_as_external_project)
    set (_prefix CC_EXTERNAL_PROJ)
    set (_options NO_TOOLS)
    set (_oneValueArgs SRC_DIR BUILD_DIR INSTALL_DIR REPO TAG QT_DIR TGT)
    set (_mutiValueArgs CMAKE_ARGS)
    cmake_parse_arguments(${_prefix} "${_options}" "${_oneValueArgs}" "${_mutiValueArgs}" ${ARGN})

    if (NOT CC_EXTERNAL_PROJ_SRC_DIR)
        message (FATAL_ERROR "The SRC_DIR parameter is not provided")
    endif () 

    if (NOT CC_EXTERNAL_PROJ_BUILD_DIR)
        set (CC_EXTERNAL_PROJ_BUILD_DIR ${CC_EXTERNAL_PROJ_SRC_DIR}/build)
    endif ()   

    if (NOT CC_EXTERNAL_PROJ_INSTALL_DIR)
        set (CC_EXTERNAL_PROJ_INSTALL_DIR ${CC_EXTERNAL_PROJ_BUILD_DIR}/install)
    endif ()  

    if (NOT CC_EXTERNAL_PROJ_REPO)
        set (CC_EXTERNAL_PROJ_REPO ${CC_EXTERNAL_DEFAULT_REPO})
    endif ()  

    if (NOT CC_EXTERNAL_PROJ_TAG)
        set (CC_EXTERNAL_PROJ_TAG ${CC_EXTERNAL_DEFAULT_TAG})
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

    include(ExternalProject)
    ExternalProject_Add(
        "${CC_EXTERNAL_PROJ_TGT}"
        PREFIX "${CC_EXTERNAL_PROJ_SRC_DIR}"
        STAMP_DIR "${CC_EXTERNAL_PROJ_SRC_DIR}"
        GIT_REPOSITORY "${CC_EXTERNAL_PROJ_REPO}"
        GIT_TAG "${CC_EXTERNAL_PROJ_TAG}"
        SOURCE_DIR "${CC_EXTERNAL_PROJ_SRC_DIR}"
        BINARY_DIR "${CC_EXTERNAL_PROJ_BUILD_DIR}"
        INSTALL_DIR "${CC_EXTERNAL_PROJ_INSTALL_DIR}"
        CMAKE_ARGS 
            ${exteral_proj_qt_dir_param} ${comms_lib_only_param}
            ${CC_EXTERNAL_PROJ_CMAKE_ARGS}
    )    

    include (CC_DefineExternalProjectTargets.cmake)

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
endfunction ()