SET(CPT_API_CLIENT_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_parse.c
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_client.c
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_serialize.c
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_packet_builder.c
        PARENT_SCOPE)

SET(CPT_API_SERVER_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_user.c
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_parse.c
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_server.c
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_channel.c
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_serialize.c
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_packet_builder.c
        PARENT_SCOPE)