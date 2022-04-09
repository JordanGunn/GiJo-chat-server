//
// Created by jordan on 2022-03-22.
//

#include "command.h"


void parse_cmd_input(Command * cmd)
{
    parse_cmd(cmd);
    parse_cmd_args(cmd);
}


void parse_cmd_args(Command * cmd)
{
    char arg_buf[SM_BUFF_SIZE] = {0};
    char * arg_start, * arg_end;

    if ( cmd->p_input )
    {
        arg_start = cmd->p_input;
        if ( !strchr(arg_start, '\"') )
        {
            arg_end = strchr(arg_start++, '\n');
        }
        else
        {
            arg_start = strchr(cmd->p_input, '\"'); arg_start++;
            arg_end = strchr(arg_start + 1, '\"');
        }

        memcpy(arg_buf, arg_start,
               ((unsigned long) (arg_end - arg_start)));

        cmd->args = strdup(arg_buf);
        cmd->p_input = NULL;
    }
}


bool is_cmd(Command * command, char * cli_cmd)
{
    return !( strcmp(command->cmd, cli_cmd) );
}


bool is_valid_cmd(Command * cmd) {

    int i;
    if ( !cmd->cmd ) { return false; }
    if (cmd->cmd[0] == '@')
    {
        for (i = 0; i < NUM_CMD; i++)
        {
            char * c = cli_cmds[i];
            if ( !(strcmp(cmd->cmd, cli_cmds[i])) )
            { return true; }
        }
    }

    return false;
}


void parse_cmd(Command * cmd)
{
    char cmd_buf[SM_BUFF_SIZE] = {0};
    char * cmd_start, * cmd_end;

    cmd_start = cmd->input;
    if ( !(cmd_end = strchr(cmd->input, ' ')) )
    {
        cmd_end = strchr(cmd->input, '\n');
        memcpy(cmd_buf, cmd_start,
               ((unsigned long) (cmd_end - cmd_start)));

        cmd->p_input = NULL;
    }
    else
    {
        memcpy(cmd_buf, cmd_start,
               (unsigned long) (cmd_end - cmd_start));

        cmd->p_input = cmd_end;
    }

    cmd->cmd = strdup(cmd_buf);
}


Command * cmd_init()
{
    Command * command;
    if ( (command = malloc(sizeof( struct command))) )
    {
        command->cmd = NULL;
        command->args = NULL;
    }

    return command;
}


void cmd_destroy(Command * cmd)
{
    if ( cmd )
    {
        if ( cmd->cmd )
        {
            free( cmd->cmd );
            cmd->cmd = NULL;
        }
        if ( cmd->args )
        {
            free(cmd->args);
            cmd->args = NULL;
        }

        if ( cmd->input )
        {
            free(cmd->input);
            cmd->input = NULL;
        }

        free(cmd);
        cmd = NULL;
    }
}


char * cmd_get_input()
{
    char buf[SM_BUFF_SIZE] = {0};

    read(STDIN_FILENO, buf, SM_BUFF_SIZE);
    return strdup(buf);
}

