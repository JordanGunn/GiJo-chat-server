add_subdirectory(include/)
add_subdirectory(src/)

SET(APP_CLIENT_TARGET
        ${CHAT_CLIENT_INCLUDE}
        ${CHAT_CLIENT_SOURCE}
        PARENT_SCOPE)