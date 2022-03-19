add_subdirectory(src)
add_subdirectory(include)

SET(TCP_CLIENT_TARGETS
        ${TCP_CLIENT_HEADERS}
        ${TCP_CLIENT_SRC}
        PARENT_SCOPE)

SET(TCP_SERVER_TARGETS
        ${TCP_SERVER_HEADERS}
        ${TCP_SERVER_SRC}
        PARENT_SCOPE)
