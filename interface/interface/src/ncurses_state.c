//
// Created by charlie on 2022-04-08.
//

#include "common.h"
#include "ncurses_state.h"


NcursesState * ncurses_state_init(void)
{
    NcursesState * nstate;

    nstate = malloc(sizeof (struct ncurses_state));

    nstate->game_window = NULL;
    nstate->chat_input_window = NULL;
    nstate->chat_dialogue_window = NULL;
    nstate->msg_history = NULL;
    nstate->msg_count = 0;

    return nstate;
}


void ncurses_state_destroy(NcursesState * nstate)
{
    if (nstate)
    {
        if ( nstate->game_window )
        {
            nstate->game_window = NULL;
        }

        if ( nstate->chat_input_window )
        {
            nstate->chat_input_window = NULL;
        }

        if ( nstate->chat_dialogue_window )
        {
            nstate->chat_dialogue_window = NULL;
        }

        if ( nstate->msg_history )
        {
            for (int i = 0; i <= nstate->msg_count; i++)
            {
                free(nstate->msg_history[i]);
            }
            free(nstate->msg_history);
            nstate->msg_history = NULL;
        }

        free(nstate);
        nstate = NULL;
    }
}
