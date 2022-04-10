//
// Created by charlie on 2022-04-08.
//

#ifndef CPT_NCURSES_STATE_H
#define CPT_NCURSES_STATE_H

#include <ncurses.h>

/**
 * NcursesState object.
 */
typedef struct ncurses_state NcursesState;
struct ncurses_state
{
    WINDOW * game_window;
    WINDOW * chat_input_window;
    WINDOW * chat_dialogue_window;
    char **msg_history;
    int msg_count;
};


/**
 * Initialize a NcursesState object.
 *
 * Allocate any necessary memory, set
 * remaining pointers to NULL, and remaining
 * values to 0.
 *
 * @return Pointer to a NcursesState object.
 */
NcursesState * ncurses_state_init(void);


/**
 * Destroy UserState object.
 *
 * Free any allocated memory if necessary,
 * and set all remaining pointer to NULL.
 */
void ncurses_state_destroy(NcursesState * nstate);


#endif //CPT_NCURSES_STATE_H
