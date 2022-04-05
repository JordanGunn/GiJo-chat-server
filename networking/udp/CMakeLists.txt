add_subdirectory(src/)
add_subdirectory(include/)

SET(UDP_CLIENT_TARGET
        ${UDP_CLIENT_SOURCE}
        ${UDP_CLIENT_INCLUDE}
        PARENT_SCOPE)

SET(UDP_SERVER_TARGET
        ${UDP_SERVER_SOURCE}
        ${UDP_SERVER_INCLUDE}
        PARENT_SCOPE)