//
// Created by jordan on 2022-03-19.
//

#ifndef CPT_CLI_DC_H
#define CPT_CLI_DC_H

#include "client_config.h"
#include "tcp_client.h"

#include "shared_memory.h"
#include "user_state.h"
#include "handlers.h"
#include "pthread.h"

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

struct application_settings
{
    struct dc_opt_settings opts;
    struct dc_setting_string * port;
    struct dc_setting_string * host;
    struct dc_setting_string * login;
};


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
void * recv_thread(void * user_state);


/**
 *
 *
 * @param cmd
 */
void recv_handler(UserState * user_state);


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
 *
 * @param data
 * @return
 */
void * send_thread(void * user_state);


/**
 *
 * @param th
 */
void thread_chat_io(pthread_t th[NUM_MSG_THREADS], UserState * ustate);


/**
 * Execute user entered commands.
 *
 * @param user_commands Commands from user input.
 */
void command_handler(UserState * ustate);


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
