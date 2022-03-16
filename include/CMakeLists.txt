set(CLIENT_HEADER_LIST
        "${CPT_SOURCE_DIR}/include/common.h"
        ${CPT_API_HEADERS}
        ${TCP_CLIENT_HEADERS}
        ${APP_CLIENT_HEADERS}
        PARENT_SCOPE)

set(SERVER_HEADER_LIST
        "${CPT_SOURCE_DIR}/include/common.h"
        ${CPT_API_HEADERS}
        ${TCP_SERVER_HEADERS}
        ${APP_SERVER_HEADERS}
        PARENT_SCOPE)