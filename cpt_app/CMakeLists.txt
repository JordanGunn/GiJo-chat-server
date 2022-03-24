add_subdirectory(client/)
add_subdirectory(server/)

SET(CHAT_CLIENT_TARGET
        ${APP_CLIENT_TARGET}
        PARENT_SCOPE)

SET(CHAT_SERVER_TARGET
        ${APP_SERVER_TARGET}
        PARENT_SCOPE)