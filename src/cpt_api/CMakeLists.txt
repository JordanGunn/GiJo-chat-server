link_directories(${CPT_SOURCE_DIR}/include/cpt_api)

SET(CPT_API_SRC
        ${CPT_SOURCE_DIR}/src/cpt_api/cpt_builder.c
        ${CPT_SOURCE_DIR}/src/cpt_api/cpt_client.c
        ${CPT_SOURCE_DIR}/src/cpt_api/cpt_server.c
        PARENT_SCOPE
        )