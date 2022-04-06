add_subdirectory(cli/)
add_subdirectory(ncurses/)

SET(CLI_INTERFACE_TARGET
        ${CLI_TARGET}
        PARENT_SCOPE)

SET(NCURSES_INTERFACE_TARGET
        ${NCURSES_TARGET}
        PARENT_SCOPE)