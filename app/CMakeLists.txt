SET(CHAT_CLIENT_TARGETS
        ${CPT_SOURCE_DIR}/app/cpt_chat_client.c
        ${CPT_SOURCE_DIR}/app/cpt_chat_client.h
        PARENT_SCOPE)

SET(CHAT_SERVER_TARGETS
        ${CPT_SOURCE_DIR}/app/cpt_chat_server.c
        ${CPT_SOURCE_DIR}/app/cpt_chat_server.h
        PARENT_SCOPE)

SET(SANDBOX_TARGETS
        ${CHAT_SERVER_TARGETS}
        ${CHAT_CLIENT_TARGETS}
        ${CPT_SOURCE_DIR}/app/sandbox.c
    PARENT_SCOPE)