//
// Created by jordan on 2022-03-22.
//

#ifndef CPT_COMMAND_H
#define CPT_COMMAND_H

#include "common.h"

#define NUM_CMD 7

/**
 * Valid CLI commands.
 */
static char * cli_cmds[] =
{
        "@send",
        "@get-users",
        "@create-channel",
        "@join-channel",
        "@leave-channel",
        "@logout",
        "@menu"
};

/**
 * Command Object.
 *
 * Holds Commands, and Command arguments
 * from users.
 */
typedef struct command Command;
struct command
{
    char * cmd;
    char * args;
    char * input;
    char * p_input;
};


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
 * Execute user entered commands.
 *
 * @param user_commands Commands from user input.
 */
void handle_command(Command * cmd);

/**
 * Parse user input from stdin.
 *
 * Calls parse_cmd_args() and parse_cmd().
 *
 * @param cmd   Pointer to Command object.
 */
void parse_cmd_input(Command * cmd);


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
 * Check if user input is a command.
 *
 * @param command
 * @param cli_cmd
 * @return
 */
bool is_cmd(Command * command, char * cli_cmd);




#endif //CPT_COMMAND_H
