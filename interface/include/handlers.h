//
// Created by jordan on 2022-03-22.
//

#ifndef CPT_HANDLERS_H
#define CPT_HANDLERS_H

#include "user_state.h"

/**
 *
 * @param ustate
 * @param name
 * @return
 */
int login_handler(UserState * ustate, char * name);


/**
 *
 * @param ustate
 */
void logout_handler(UserState * ustate);


/**
 *
 * @param ustate
 */
void get_users_handler(UserState * ustate);


/**
 *
 * @param ustate
 */
void join_channel_handler(UserState * ustate);


/**
 * Handle CREATE_CHANNEL cpt protocol request and response.
 *
 * @param ustate
 */
void create_channel_handler(UserState * ustate);

/**
 *
 * @param ustate
 */
void leave_channel_handler(UserState * ustate);


/**
 *
 * @param ustate
 */
void send_handler(UserState * ustate);


#endif //CPT_HANDLERS_H
