add_subdirectory(src/)
add_subdirectory(include/)

SET(CLI_TARGET
        ${INTERFACE_SOURCE}
        ${INTERFACE_INCLUDE}
        PARENT_SCOPE)