find_package (Doxygen)
if (DOXYGEN_FOUND)
    set (doc_output_dir "${CMAKE_INSTALL_PREFIX}/${DOC_INSTALL_DIR}/comms")
    
    set (match_str "OUTPUT_DIRECTORY[^\n]*")
    set (replacement_str "OUTPUT_DIRECTORY = ${doc_output_dir}")
    set (output_file "${CMAKE_CURRENT_BINARY_DIR}/doxygen.conf")

    set (config_file "${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen.conf")
    file (READ ${config_file} config_text)
    string (REGEX REPLACE "${match_str}" "${replacement_str}" modified_config_text "${config_text}")
    file (WRITE "${output_file}" "${modified_config_text}")

    add_custom_target ("doc_comms"
            COMMAND ${CMAKE_COMMAND} -E make_directory ${doc_output_dir}
            COMMAND ${DOXYGEN_EXECUTABLE} ${output_file}
            COMMAND ${CMAKE_COMMAND} -DDOC_OUTPUT_DIR="${doc_output_dir}" -P 
                            ${PROJECT_SOURCE_DIR}/cmake/CC_DocCleanupScript.cmake
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif ()

add_library(comms INTERFACE)
add_library (cc::comms ALIAS comms)

set (sys_prefix)
#if (CC_EXPORT_AS_SYSTEM_HEADERS)
#    set (sys_prefix SYSTEM)
#endif ()

target_include_directories(comms ${sys_prefix} INTERFACE
  $<INSTALL_INTERFACE:include>
)

target_include_directories(comms INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_options(comms INTERFACE
  $<$<CXX_COMPILER_ID:MSVC>:/wd4503 /wd4309 /wd4267 -D_SCL_SECURE_NO_WARNINGS>
)

install(TARGETS comms EXPORT commsExport)
install(EXPORT commsExport NAMESPACE cc::
  DESTINATION ${LIB_INSTALL_DIR}/LibComms/cmake
)

install (
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/comms
    DESTINATION ${INC_INSTALL_DIR}
)

add_subdirectory (test)

FILE(GLOB_RECURSE headers "*.h")
add_custom_target(comms.headers SOURCES ${headers})

