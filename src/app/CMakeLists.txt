link_directories(${CPT_SOURCE_DIR}/include/app)

set(APP_CLIENT_SRC
        ${CPT_SOURCE_DIR}/src/app/cpt_chat_client.c
        PARENT_SCOPE)

set(APP_SERVER_SRC
        ${CPT_SOURCE_DIR}/src/app/cpt_chat_server.c
        PARENT_SCOPE)