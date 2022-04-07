add_subdirectory(src/)
add_subdirectory(include/)

SET(INTERFACE_TARGET
        ${INTERFACE_INCLUDE}
        ${INTERFACE_SOURCE}
        PARENT_SCOPE)