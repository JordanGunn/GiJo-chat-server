SET(CPT_VOICE_SERVER_TARGET
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_voice_server.c
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_voice_server.h
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_voice_definitions.h
        PARENT_SCOPE)


SET(CPT_VOICE_CLIENT_TARGET
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_voice_client.c
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_voice_client.h
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_voice_definitions.h
        PARENT_SCOPE)