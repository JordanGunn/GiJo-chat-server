//
// Created by jordan on 2022-03-22.
//

#ifndef CPT_HANDLERS_H
#define CPT_HANDLERS_H

#include "user_state.h"

/**
 *
 *
 * @param ustate    Pointer to a UserState object.
 * @param name      Username, if any.
 * @return
 */
int login_handler(UserState * ustate, char * name);


/**
 *
 * @param ustate    Pointer to a UserState object.
 */
void logout_handler(UserState * ustate);


/**
 *
 * @param ustate    Pointer to a UserState object.
 */
void get_users_handler(UserState * ustate);


/**
 *
 * @param ustate    Pointer to a UserState object.
 */
void join_channel_handler(UserState * ustate);


/**
 * Handle CREATE_CHANNEL cpt protocol request and response.
 *
 * @param ustate    Pointer to a UserState object.
 */
void create_channel_handler(UserState * ustate);


/**
 *
 * @param ustate    Pointer to a UserState object.
 */
void leave_channel_handler(UserState * ustate);


/**
 * Handle transmission of serialized CptRequest object.
 *
 * @param ustate    Pointer to a UserState object.
 */
void send_handler(UserState * ustate);


/**
 * Handle incoming Cpt Response from server.
 *
 * @param ustate    Pointer to a UserState object.
 * @param res       CptResponse object parsed from serial data.
 */
void recv_handler(UserState *ustate, const CptResponse * res);


/**
 * Execute user entered commands.
 *
 * @param user_commands Commands from user input.
 */
void cmd_handler(UserState * ustate);


#endif //CPT_HANDLERS_H
