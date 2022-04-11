//
// Created by jordan on 2022-03-22.
//

#ifndef CPT_HANDLERS_H
#define CPT_HANDLERS_H

#include "user_state.h"

#define BUFF_SIZE 1024
#define MSG_HISTORY_SIZE 100

#define GAME_WINDOW "Game Window:"
#define CHAT_WINDOW "Chat Input Window:"
#define DIALOGUE_WINDOW "Chat Dialogue Window:"

void update_msg_history(char msg[], char **msg_history, int *msg_count);
void clear_window(WINDOW *window, char *label);
void display_input(WINDOW * chat_dialogue_input, char **msg_history, const int *msg_count);
void update_window(WINDOW *window, char *label);

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
 *
 * @param ustate
 */
void create_vchannel_handler(UserState * ustate);


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




/**
 * Check if protocol version in packet is voice compatible.
 *
 * @param ustate    Pointer to a UserState object.
 * @return true or false;
 */
bool is_voice_compat(UserState * ustate);


/**
 * Check if channel is within voice channel bounds.
 *
 * @param ustate    Pointer to a UserState object.
 * @return true or false.
 */
bool is_voice_chan(UserState * ustate);


/**
 * Determine whether or not to start up voice.
 *
 * @param ustate    Pointer to a UserState object.
 * @return true or false.
 */
bool is_voice(UserState * ustate);


#endif //CPT_HANDLERS_H
