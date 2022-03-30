//
// Created by jordan on 2022-03-30.
//

#ifndef CPT_USER_STATE_H
#define CPT_USER_STATE_H

#include "command.h"
#include "cpt_client.h"


/**
 * UserState object.
 */
typedef struct user_state UserState;
struct user_state
{
    Command * cmd;
    uint16_t channel;
    CptClientInfo * client_info;

    bool LOGGED_IN;
};


/**
 *
 * @return
 */
UserState * user_state_init();


/**
 *
 */
void user_state_destroy();


#endif //CPT_USER_STATE_H
