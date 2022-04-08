//
// Created by jordan on 2022-03-30.
//

#ifndef CPT_USER_STATE_H
#define CPT_USER_STATE_H

#include "common.h"
#include "command.h"
#include "cpt_voice.h"
#include "cpt_client.h"
/**
 * UserState object.
 */
typedef struct user_state UserState;
struct user_state
{
    int pid;
    Command * cmd;
    uint16_t channel;
    ClientInfo * client_info;

    bool LOGGED_IN;
};


/**
 * Initialize a UserState object.
 *
 * Allocate any necessary memory, set
 * remaining pointers to NULL, and remaining
 * values to 0.
 *
 * @return Pointer to a UserSState object.
 */
UserState * user_state_init(void);


/**
 * Destroy UserState object.
 *
 * Free any allocated memory if necessary,
 * and set all remaining pointer to NULL.
 */
void user_state_destroy(UserState * ustate);


#endif //CPT_USER_STATE_H
