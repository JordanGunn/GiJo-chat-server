add_subdirectory(src)
add_subdirectory(include)
#
#add_library(cpt_api

#        )
#
#target_include_directories(cpt_api PUBLIC "${CPT_SOURCE_DIR}/cpt_api/src")
#target_include_directories(cpt_api PUBLIC "${CPT_SOURCE_DIR}/cpt_api/include")

SET(CPT_API_TARGETS
    ${CPT_API_SRC}
    ${CPT_API_HEADERS}
    PARENT_SCOPE)



