add_subdirectory(src/)
add_subdirectory(include/)

SET(CLI_TARGET
        ${CLI_SOURCE}
        ${CLI_INCLUDE}
        PARENT_SCOPE)