add_subdirectory(tcp/)
add_subdirectory(udp/)

SET(NETWORK_CLIENT_TARGET
        ${UDP_NETWORK_TARGET}
        ${TCP_CLIENT_TARGET}
        PARENT_SCOPE)

SET(NETWORK_SERVER_TARGET
        ${UDP_NETWORK_TARGET}
        ${TCP_SERVER_TARGET}
        PARENT_SCOPE)