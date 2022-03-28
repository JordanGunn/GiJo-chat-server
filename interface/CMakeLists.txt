add_subdirectory(src/)
add_subdirectory(include/)

SET(CLI_TARGET
        ${INTERFACE_SOURCE}
        ${INTERFACE_INCLUDE}
        ${CMAKE_CURRENT_SOURCE_DIR}/cli_dc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/cli_dc.h
        PARENT_SCOPE)