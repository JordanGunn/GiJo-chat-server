add_subdirectory(src/)
add_subdirectory(include/)
add_subdirectory(cpt_voice/)

SET(CPT_API_SERVER_TGT
        ${CPT_API_SERVER_INCLUDE}
        ${CPT_API_SERVER_SRC}
        PARENT_SCOPE)

SET(CPT_API_CLIENT_TGT
        ${CPT_API_CLIENT_INCLUDE}
        ${CPT_API_CLIENT_SRC}
        ${CPT_VOICE_TARGET}
        PARENT_SCOPE)



