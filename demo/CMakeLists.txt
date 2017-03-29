find_package (Doxygen)
if (DOXYGEN_FOUND)
    set (doc_output_dir "${DOC_INSTALL_DIR}/demo")
    make_directory (${doc_output_dir})
    
    set (match_str "OUTPUT_DIRECTORY[^\n]*")
    set (replacement_str "OUTPUT_DIRECTORY = ${doc_output_dir}")
    set (output_file "${CMAKE_CURRENT_BINARY_DIR}/doxygen.conf")

    set (config_file "${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen.conf")
    file (READ ${config_file} config_text)
    string (REGEX REPLACE "${match_str}" "${replacement_str}" modified_config_text "${config_text}")
    file (WRITE "${output_file}" "${modified_config_text}")

    add_custom_target ("doc_demo"
            COMMAND ${DOXYGEN_EXECUTABLE} ${output_file}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif ()

install (
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/demo
    DESTINATION ${INC_INSTALL_DIR}
)

include_directories (
    BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory (cc_plugin)

install (
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/config/
    DESTINATION ${CONFIG_INSTALL_DIR}
)

FILE(GLOB_RECURSE headers "include/*.h")
add_custom_target(demo.headers SOURCES ${headers})
