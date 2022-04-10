//
// Created by jordan on 2022-03-30.
//

#include "user_state.h"


UserState * user_state_init(void)
{
    UserState * ustate;

    ustate = malloc(sizeof (struct user_state));
    ustate->client_info = NULL;
    ustate->cmd = NULL;
    ustate->channel = 0;
    ustate->ncurses_state = NULL;

    return ustate;
}


void user_state_destroy(UserState * ustate)
{
    if (ustate)
    {
        if (ustate->cmd)
        {
            cmd_destroy(ustate->cmd);
        }

        if (ustate->client_info)
        {
            cpt_destroy_client_info(ustate->client_info);
        }

        if (ustate->ncurses_state)
        {
            ncurses_state_destroy(ustate->ncurses_state);
        }

        free(ustate);
        ustate = NULL;
    }
}
