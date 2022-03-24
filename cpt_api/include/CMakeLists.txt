SET(CPT_API_CLIENT_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_types.h
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_parse.h
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_client.h
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_serialize.h
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_definitions.h
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_packet_builder.h
        PARENT_SCOPE)

SET(CPT_API_SERVER_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_user.h
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_types.h
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_parse.h
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_server.h
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_channel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_serialize.h
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_definitions.h
        ${CMAKE_CURRENT_SOURCE_DIR}/cpt_packet_builder.h
        PARENT_SCOPE)

