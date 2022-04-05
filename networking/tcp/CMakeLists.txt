add_subdirectory(src/)
add_subdirectory(include/)

SET(TCP_CLIENT_TARGET
        ${TCP_CLIENT_INCLUDE}
        ${TCP_CLIENT_SOURCE}
        PARENT_SCOPE)

SET(TCP_SERVER_TARGET
        ${TCP_SERVER_INCLUDE}
        ${TCP_SERVER_SOURCE}
        PARENT_SCOPE)
