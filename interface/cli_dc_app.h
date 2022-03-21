//
// Created by jordan on 2022-03-19.
//

#ifndef CPT_CLI_DC_APP_H
#define CPT_CLI_DC_APP_H

#include "client_config.h"
#include "cpt_client.h"
#include "tcp_client.h"

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

#define NUM_CMD 6

typedef void (* Prompt)(void);
typedef struct user_commands UserCommands;
typedef struct user_state UserState;

static char * cli_commands[] = {
        "--SEND",
        "--GET-USERS",
        "--CREATE-CHANNEL",
        "--JOIN-CHANNEL",
        "--LEAVE-CHANNEL",
        "--LOGOUT"
};

struct user_state
{
    char * name;
    bool LOGGED_IN;
    uint16_t channel;
    CptClientInfo * client_info;
};

struct application_settings
{
    struct dc_opt_settings opts;
    struct dc_setting_string * port;
    struct dc_setting_string * host;
    struct dc_setting_string * login;
    struct dc_setting_string * logout;
    struct dc_setting_string * send;
    struct dc_setting_string * get_users;
    struct dc_setting_string * create_channel;
    struct dc_setting_string * join_channel;
    struct dc_setting_string * leave_channel;
};


/**
 * Send logout request to server.
 *
 * @return -1 if failure.
 */
int user_logout();


/**
 * Handle user_logout() function call.
 *
 * Handles the logout function call and destroys
 * any necessary memory from the program.
 *
 */
void logout_handler();


/**
 * Make login request to server.
 *
 * @param name  Name of requesting user.
 * @return 0 if successful, -1 if failure.
 */
int user_login(char *name);


/**
 * Call the user_login() function and control result.
 *
 * @param host  Server host address.
 * @param port  Server host port.
 * @param login Name of requested login user.
 */
void login_handler(char * host, char * port, char * name);


/**
 * Execute user entered commands.
 *
 * @param user_commands Commands from user input.
 */
void handle_commands(char * user_commands[]);


/**
 * Print the menu and all CLI options.
 */
void menu();


/**
 * Count commands entered by user.
 *
 * @param commands  Commands gather from application framework.
 * @return Number of commands that are not NULL.
 */
int count_commands(char * commands[NUM_CMD]);


/**
 * Get user input from STDIN.
 *
 * @return
 */
char * get_user_input();


/**
 * Check user input against valid CLI commands.
 *
 * @param input     string from get_user_input().
 * @return
 */
bool is_valid_command(char *input);



// =====================================================================================
// D C   A P P L I CA T I O N   F R A M E WO R K    I N T E R FA C E   F U N C T I O N S
// =====================================================================================

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

#endif //CPT_CLI_DC_APP_H
