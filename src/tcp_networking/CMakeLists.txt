link_directories(${CPT_SOURCE_DIR}/include/tcp_networking)

SET(TCP_CLIENT_SRC
        ${CPT_SOURCE_DIR}/src/tcp_networking/tcp_client.c
        PARENT_SCOPE
        )

SET(TCP_SERVER_SRC
        ${CPT_SOURCE_DIR}/src/tcp_networking/tcp_server.c
        PARENT_SCOPE
        )