add_subdirectory(src/)
add_subdirectory(include/)

SET(UDP_NETWORK_TARGET
        ${UDP_CLIENT_SOURCE}
        ${UDP_CLIENT_INCLUDE}
        ${UDP_SERVER_SOURCE}
        ${UDP_SERVER_INCLUDE}
        PARENT_SCOPE)