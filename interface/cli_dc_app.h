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

#define NUM_CMD 7

#define SEND 0
#define GET_USERS 1
#define CREATE_CHANNEL 2
#define JOIN_CHANNEL 3
#define LEAVE_CHANNEL 4
#define LOGOUT 5
#define MENU 6


typedef void (* Prompt)(void);
typedef struct command Command;
typedef struct user_state UserState;


static char * cli_cmds[] =
{
        "send",
        "get-users",
        "create-channel",
        "join-channel",
        "leave-channel",
        "logout",
        "menu"
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
};


struct command
{
    char * cmd;
    void * args;
    char * input;
    char * p_input;
};

typedef struct command Command;


/**
 * Check user input against valid CLI commands.
 *
 * @param cmd     string from get_user_input().
 * @return true or false.
 */
bool is_valid_cmd(Command * cmd);


/**
 * Initialize command object.
 *
 * Allocates necessary memory and initialize all
 * pointers to NULL.
 *
 * @return Pointer to command object.
 */
Command * cmd_init();


/**
 * Destroy Command object.
 *
 * Free any dynamically allocated memory
 * and set all pointer to NULL.
 *
 * @param cmd   Pointer to Command object.
 */
void cmd_destroy(Command * cmd);


/**
 * Make LOGIN request to server.
 *
 * @param name  Name of requesting user.
 * @return 0 if successful, -1 if failure.
 */
int login_handler(char *name);


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
void get_users_handler(Command * cmd);


/**
 * Call the login_handler() function and control result.
 *
 * @param host  Server host address.
 * @param port  Server host port.
 * @param login Name of requested login user.
 */
void user_login(char * host, char * port, char * name);


/**
 * Execute user entered commands.
 *
 * @param user_commands Commands from user input.
 */
void handle_commands(Command * command);


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
 * Parse user input from stdin.
 *
 * Calls parse_cmd_args() and parse_cmd().
 *
 * @param cmd   Pointer to Command object.
 */
void parse_user_input(Command * cmd);


/**
 * Print prompt to stdout.
 */
void chat_prompt();


/**
 * Parse command from user input.
 *
 * @param cmd   Pointer to Command object.
 */
void parse_cmd(Command * cmd);


/**
 * Parse arguments from user input.
 *
 * @param cmd   Pointer to Command object.
 */
void parse_cmd_args(Command * cmd);


/**
 * Handle parsed commands.
 *
 * @param cmd   Pointer to Command object.
 */
void handle_cmd(Command * cmd);


/**
 * Handle CREATE_CHANNEL cpt protocol request and response.
 *
 * @param cmd   Pointer to command object.
 */
void create_channel_handler(Command * cmd);


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

#endif //CPT_CLI_DC_APP_H
