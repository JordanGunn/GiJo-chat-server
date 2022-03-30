//
// Created by jordan on 2022-03-19.
//

#ifndef CPT_CLI_DC_H
#define CPT_CLI_DC_H

#include "client_config.h"
#include "cpt_client.h"
#include "tcp_client.h"

#include "command.h"
#include "pthread.h"
#include "shared_memory.h"

#include <dc_application/command_line.h>
#include <dc_application/config.h>
#include <dc_application/defaults.h>
#include <dc_application/environment.h>
#include <dc_application/options.h>
#include <dc_posix/dc_stdlib.h>
#include <dc_posix/dc_string.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_MSG_THREADS 2

typedef struct command Command;
typedef struct user_state UserState;


struct user_state
{
    Command * cmd;
    uint16_t channel;
    CptClientInfo * client_info;
};


struct application_settings
{
    struct dc_opt_settings opts;
    struct dc_setting_string * port;
    struct dc_setting_string * host;
    struct dc_setting_string * login;
};

/**
 *
 * @return
 */
UserState * user_state_init();

/**
 * Make LOGIN request to server.
 *
 * @param name  Name of requesting user.
 * @return 0 if successful, -1 if failure.
 */
int login_handler(UserState * ustate, char * name);


/**
 * Make a LOGOUT request to the server.
 *
 * Handles the logout function call and destroys
 * any necessary memory from the program.
 *
 */
void logout_handler();


/**
 * Make GET_USERS request to the server.
 *
 * @param cmd
 */
void get_users_handler(UserState * ustate);


/**
 *
 */
void join_channel_handler(UserState * ustate);


/**
 * Call the login_handler() function and control result.
 *
 * @param host  Server host address.
 * @param port  Server host port.
 * @param login Name of requested login user.
 */
void user_login(UserState * ustate, char * host, char * port, char * name);


/**
 *
 *
 * @param cmd
 */
void send_handler(UserState * ustate, char * msg);


/**
 *
 *
 * @param cmd
 */
void recv_handler(UserState * ustate, uint8_t code);


/**
 * Print the menu and all CLI options.
 */
void menu();


/**
 * Get user input from STDIN.
 *
 * @return Input from user as char pointer.
 */
char * get_user_input();


/**
 * Handle CREATE_CHANNEL cpt protocol request and response.
 *
 * @param cmd   Pointer to command object.
 */
void create_channel_handler(UserState * ustate);

/**
 *
 * @param ustate
 */
void leave_channel_handler(UserState * ustate);


/**
 *
 * @param data
 * @return
 */
void * send_thread(void * data);


/**
 *
 * @param data
 * @return
 */
void * command_thread(void * data);


/**
 *
 * @param th
 */
void thread_msgs(pthread_t th[NUM_MSG_THREADS], UserState * ustate);


/**
 *
 */
void recv_handler(UserState * ustate);



void chat_prompt(UserState * ustate);


/**
 *
 */
void destroy_cli_user();

// ========================================================================================
// D C   A P P L I C A T I O N   F R A M E W O R K    I N T E R F A C E   F U N C T I O N S
// ========================================================================================

/**
 *  DC application framework interface function.
 */
struct dc_application_settings *create_settings(const struct dc_posix_env * env, struct dc_error * err);


/**
 *  DC application framework interface function.
 */
int destroy_settings(const struct dc_posix_env * env, struct dc_error * err,
                     struct dc_application_settings ** psettings);


/**
 *  DC application framework interface function.
 */
int run(const struct dc_posix_env * env, struct dc_error *err, struct dc_application_settings * settings);


/**
 *  DC application framework interface function.
 */
void error_reporter(const struct dc_error * err);


/**
 *  DC application framework interface function.
 */
void trace_reporter(const struct dc_posix_env *env, const char *file_name,
                    const char *function_name, size_t line_number);


/**
 *  DC application framework interface function.
 */
int run_client_cli(int argc, char *argv[]);

#endif //CPT_CLI_DC_H
