add_subdirectory(include/)
add_subdirectory(src/)

SET(APP_SERVER_TARGET
        ${CHAT_SERVER_INCLUDE}
        ${CHAT_SERVER_SOURCE}
        PARENT_SCOPE)