add_subdirectory(src/)
add_subdirectory(include/)

SET(NCURSES_TARGET
        ${NCURSES_SOURCE}
        ${NCURSES_INCLUDE}
        PARENT_SCOPE)