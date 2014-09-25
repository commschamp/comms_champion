cmake_minimum_required (VERSION 2.8.11)
project ("comms_champion")

set (EXTERNALS_DIR "${CMAKE_SOURCE_DIR}/external")
find_package (Git REQUIRED)
 
execute_process (
    COMMAND ${GIT_EXECUTABLE} submodule update --init
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
 
set (CMAKE_MODULE_PATH "${EXTERNALS_DIR}/cmake")         
include (AllExtras)

