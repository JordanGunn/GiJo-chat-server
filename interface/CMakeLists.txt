add_subdirectory(cli/)
add_subdirectory(ncurses/)
add_subdirectory(interface/)

#SET(CLI_INTERFACE_TARGET
#        ${CLI_TARGET}
#        ${INTERFACE_TARGET}
#        PARENT_SCOPE)

SET(NCURSES_INTERFACE_TARGET
        ${CLI_TARGET}
        ${NCURSES_TARGET}
        ${INTERFACE_TARGET}
        PARENT_SCOPE)