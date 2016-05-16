# Expects DOC_OUTPUT_DIR to be set

file(GLOB_RECURSE all_doc_files "${DOC_OUTPUT_DIR}/*.html")

foreach (f ${all_doc_files})
    file (READ ${f} f_text)
    string (REPLACE "${DOC_OUTPUT_DIR}/html/" "" modified_f_text "${f_text}")
    file (WRITE "${f}" "${modified_f_text}")
endforeach ()
