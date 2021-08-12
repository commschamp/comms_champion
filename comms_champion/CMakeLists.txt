find_package (Doxygen)
if (DOXYGEN_FOUND)
    set (doc_output_dir "${CMAKE_INSTALL_PREFIX}/${DOC_INSTALL_DIR}/comms_champion")
    set (match_str "OUTPUT_DIRECTORY[^\n]*")
    set (replacement_str "OUTPUT_DIRECTORY = ${doc_output_dir}")
    set (output_file "${CMAKE_CURRENT_BINARY_DIR}/doxygen.conf")

    set (config_file "${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen.conf")
    file (READ ${config_file} config_text)
    string (REGEX REPLACE "${match_str}" "${replacement_str}" modified_config_text "${config_text}")
    file (WRITE "${output_file}" "${modified_config_text}")

    add_custom_target ("doc_commschampion"
            COMMAND ${CMAKE_COMMAND} -E make_directory ${doc_output_dir}
            COMMAND ${DOXYGEN_EXECUTABLE} ${output_file}
            COMMAND ${CMAKE_COMMAND} -DDOC_OUTPUT_DIR="${doc_output_dir}" -P 
                            ${PROJECT_SOURCE_DIR}/cmake/CC_DocCleanupScript.cmake
            COMMAND ${CMAKE_COMMAND} -E copy_directory 
                    ${CMAKE_CURRENT_SOURCE_DIR}/doc/image ${doc_output_dir}/html/image
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif ()

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/include
)

if (NOT CC_BUILD_TOOLS_LIBRARY)
    return ()
endif ()    

add_subdirectory (lib)

if (NOT CC_BUILD_TOOLS)
    return ()
endif ()

add_subdirectory (plugin)
add_subdirectory (app)
