SET(CHAT_CLIENT_TARGETS
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_chat_client.c
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_chat_client.h
        PARENT_SCOPE)

SET(CHAT_SERVER_TARGETS
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_chat_server.c
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_chat_server.h
        PARENT_SCOPE)

SET(SANDBOX_TARGETS
        ${CHAT_SERVER_TARGETS}
        ${CHAT_CLIENT_TARGETS}
        ${CMAKE_CURRENT_SOURCE_DIR}/sandbox.c
        PARENT_SCOPE)