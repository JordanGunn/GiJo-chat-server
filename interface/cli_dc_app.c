//
// Created by jordan on 2022-02-10.
//

#include "cli_dc_app.h"

// =============================
// I N I T   U S E R   S T A T E
// =============================
static UserState user =
{
    .LOGGED_IN =  false,
};
// ===========================


int run_client_cli(int argc, char *argv[])
{
    dc_error_reporter reporter;
    dc_posix_tracer tracer;
    struct dc_posix_env env;
    struct dc_error err;
    struct dc_application_info *info;
    int ret_val;

    reporter = error_reporter;
    tracer = NULL;
    dc_error_init(&err, reporter);
    dc_posix_env_init(&env, tracer);
    info = dc_application_info_create(&env, &err, "Settings Application");
    ret_val = dc_application_run(&env, &err, info, create_settings, destroy_settings, run, dc_default_create_lifecycle, dc_default_destroy_lifecycle, NULL, argc, argv);
    dc_application_info_destroy(&env, &info);
    dc_error_reset(&err);

    return ret_val;
}

struct dc_application_settings * create_settings(const struct dc_posix_env *env, struct dc_error *err)
{
    struct application_settings *settings;

    DC_TRACE(env);
    settings = dc_malloc(env, err, sizeof(struct application_settings));

    if(settings == NULL)
    {
        return NULL;
    }

    settings->opts.parent.config_path = dc_setting_path_create(env, err);

    settings->host           = dc_setting_string_create(env, err);
    settings->port           = dc_setting_string_create(env, err);
    settings->login          = dc_setting_string_create(env, err);

    struct options opts[] = {
            {(struct dc_setting *)settings->opts.parent.config_path,
                    dc_options_set_path,
                    "config",
                    required_argument,
                    'c',
                    "CONFIG",
                    dc_string_from_string,
                    NULL,
                    dc_string_from_config,
                    NULL},
            {(struct dc_setting *)settings->login,
                    dc_options_set_string,
                    "LOGIN",
                    required_argument,
                    'L',
                    "LOGIN",
                    dc_string_from_string,
                    "LOGIN",
                    dc_string_from_config,
                    NULL},
            {(struct dc_setting *)settings->host,
                    dc_options_set_string,
                    "host",
                    required_argument,
                    'h',
                    "HOST",
                    dc_string_from_string,
                    "host",
                    dc_string_from_config,
                    "127.0.0.1"},
            {(struct dc_setting *)settings->port,
                    dc_options_set_string,
                    "port",
                    required_argument,
                    'p',
                    "PORT",
                    dc_string_from_string,
                    "port",
                    dc_string_from_config,
                    "8080"},
    };

    // note the trick here - we use calloc and add 1 to ensure the last line is all 0/NULL
    settings->opts.opts_count = (sizeof(opts) / sizeof(struct options)) + 1;
    settings->opts.opts_size = sizeof(struct options);
    settings->opts.opts = dc_calloc(env, err, settings->opts.opts_count, settings->opts.opts_size);
    dc_memcpy(env, settings->opts.opts, opts, sizeof(opts));
    settings->opts.flags = "L:h:p:";
    settings->opts.env_prefix = "CPT_CHAT_CLIENT_";

    return (struct dc_application_settings *)settings;
}


bool is_cmd(Command *command, char *cli_cmd);

int destroy_settings(const struct dc_posix_env *env, __attribute__((unused)) struct dc_error *err,
                     struct dc_application_settings **psettings)
{
    struct application_settings *app_settings;

    DC_TRACE(env);
    app_settings = (struct application_settings *)*psettings;

    dc_setting_string_destroy(env, &app_settings->host );
    dc_setting_string_destroy(env, &app_settings->port );
    dc_setting_string_destroy(env, &app_settings->login);

    dc_free(env, app_settings->opts.opts, app_settings->opts.opts_count);
    dc_free(env, *psettings, sizeof(struct application_settings));

    if(env->null_free)
    {
        *psettings = NULL;
    }

    return 0;
}


int run(const struct dc_posix_env * env, struct dc_error * err, struct dc_application_settings *settings)
{
    char * host, * port, * login;
    Command * command;

    struct application_settings * app_settings;
    app_settings = (struct application_settings *) settings;

    host  = dc_setting_string_get(env, app_settings->host );
    port  = dc_setting_string_get(env, app_settings->port );
    login = dc_setting_string_get(env, app_settings->login);

    if ( !user.LOGGED_IN )
    {
        user_login(host, port, login);
    }

    while ( user.LOGGED_IN )
    {
        command = cmd_init();
        while ( !is_valid_cmd(command) )
        {
            chat_prompt();
            command->input = get_user_input();
            parse_user_input(command);
        }
        handle_cmd(command);
        cmd_destroy(command);
    }
    close(user.client_info->fd);  // close the connection
    return EXIT_SUCCESS;
}


int login_handler(char * name)
{
    int result;
    CptResponse * res;
    size_t res_size, req_size;
    uint8_t res_buf[LG_BUFF_SIZE] = {0};
    uint8_t req_buf[LG_BUFF_SIZE] = {0};

    // send data to server
    req_size = cpt_login(user.client_info, name, req_buf);
    result = tcp_client_send(user.client_info->fd, req_buf, req_size);
    user.client_info->channel = CHANNEL_ZERO;
    user.client_info->name = strdup(name);

    if (result != SYS_CALL_FAIL)
    {
        res_size = tcp_client_recv(user.client_info->fd, res_buf);
        res = cpt_parse_response(res_buf, res_size);
        if ( res )
        {
            if (res->code == SUCCESS)
            {
                printf("\n\nLogged in to chat server as %s@%s:%s\n\n",
                       name, user.client_info->ip, user.client_info->port);
            }
            else
            {
                printf("Error from server with code %d\n", res->code);
            }
            cpt_response_destroy(res);
        }
    }

    return result;
}


void logout_handler()
{
    size_t req_size;
    uint8_t req_buf[LG_BUFF_SIZE] = {0};

    req_size = cpt_logout(user.client_info, req_buf);
    tcp_client_send(user.client_info->fd, req_buf, req_size);
    cpt_destroy_client_info(user.client_info);
    user.LOGGED_IN = false;
}


void create_channel_handler(Command * cmd)
{
    int result;
    CptResponse * res;
    uint16_t new_cid;
    size_t req_size, res_size;
    uint8_t req_buf[MD_BUFF_SIZE] = {0};
    uint8_t res_buf[MD_BUFF_SIZE] = {0};

    req_size = cpt_create_channel(
            user.client_info, req_buf, (char *) cmd->args);

    result = tcp_client_send(
            user.client_info->fd, req_buf, req_size);

    if ( result != SYS_CALL_FAIL )
    {
        res_size = tcp_client_recv(
                user.client_info->fd, res_buf);

        res = cpt_parse_response(res_buf, res_size);
        if ( res->code == SUCCESS )
        {
            new_cid = (uint16_t) ( *(res->data) ); // new channel id is in response
            user.channel = new_cid;
            push_data(user.client_info->channels, &new_cid, sizeof(uint16_t));
            printf("\nSuccessfully created channel with users: [ %s ]\n",
                   (char *)cmd->args);
        } else { printf("Failed to create channel with code: %d", res->code); }
    }
}


void get_users_handler(Command * cmd)
{
    int result;
    CptResponse * res;
    size_t req_size, res_size;
    uint8_t req_buf[MD_BUFF_SIZE] = {0};
    uint8_t res_buf[LG_BUFF_SIZE] = {0};

    req_size = cpt_get_users( /* !!! This needs to be changed for different channels !!! */
            user.client_info, req_buf, CHANNEL_ZERO);

    result = tcp_client_send(
            user.client_info->fd, req_buf, req_size);

    if ( result != SYS_CALL_FAIL )
    {
        res_size = tcp_client_recv(
                user.client_info->fd, res_buf);

        res = cpt_parse_response(res_buf, res_size);
        if ( res->code == SUCCESS )
        {
            printf("%s\n", (char *)res->data);
        } else { printf("Failed to get users with code: %d\n", res->code); }
    }
}


void handle_cmd(Command * cmd)
{
    if ( is_cmd(cmd, cli_cmds[MENU]           )) { menu();                      }
    if ( is_cmd(cmd, cli_cmds[LOGOUT]         )) { logout_handler();            }
    if ( is_cmd(cmd, cli_cmds[SEND]           )) { puts("SEND");                }
    if ( is_cmd(cmd, cli_cmds[GET_USERS]      )) { get_users_handler(cmd);      }
    if ( is_cmd(cmd, cli_cmds[CREATE_CHANNEL] )) { create_channel_handler(cmd); }
    if ( is_cmd(cmd, cli_cmds[JOIN_CHANNEL]   )) { puts("JOIN_CHANNEL");        }
    if ( is_cmd(cmd, cli_cmds[LEAVE_CHANNEL]  )) { puts("LEAVE_CHANNEL");       }
    if ( is_cmd(cmd, cli_cmds[LEAVE_CHANNEL]  )) { puts("GET_USERS");           }
}


void user_login(char * host, char * port, char * name)
{
    int fd;

    if ( name )
    {
        user.client_info = cpt_init_client_info(port, host);
        fd = tcp_init_client(host, port);
        user.client_info->fd = fd;

        if (login_handler(name) < 0 )
        {
            printf("Failed to login to chat...\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            user.LOGGED_IN = true;
            user.name = strdup(name);
            user.channel = CHANNEL_ZERO;
        }
    }
    else
    {
        printf("User not logged in!\n");
        exit(EXIT_FAILURE);
    }
}


void menu()
{
    char menu_buf[XL_BUFF_SIZE] = {0};
    static char * logout, * get_users, * create_channel, * join_channel;
    static char * send, * title, * div, * leave_channel, * menu;

    div = "==================================================";
    title = "Choose from the following options...\n\n";
    send           = "  [0] send <message>\n";
    get_users      = "  [1] get-users <chan_id>\n";
    create_channel = "  [2] create-channel \"<uid-1> <uid-2>.. <uid-n>\"\n";
    join_channel   = "  [3] join-channel <chan_id>\n";
    leave_channel  = "  [4] leave-channel <chan_id>\n";
    logout         = "  [5] logout <name>\n";
    menu           = "  [6] menu\n";

    sprintf(menu_buf, "%s\n%s%s%s%s%s%s%s%s%s\n",
            div,
            title, send, get_users, create_channel,
            join_channel, leave_channel, logout, menu,
            div
    );

    printf("%s", menu_buf);
    printf("\n");
    fflush(stdout);
}


void chat_prompt()
{
    uint16_t channel;
    char * name;
    name = strdup(user.name);
    channel = user.channel;

    printf("[ %s | (channel %hu) ] $ ", name, channel);
    fflush(stdout);
}


char * get_user_input()
{
    char buf[SM_BUFF_SIZE];

    read(STDIN_FILENO, buf, SM_BUFF_SIZE);
    return strdup(buf);
}


void parse_user_input(Command * cmd)
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
        memcpy(arg_buf, arg_start, arg_end - arg_start);
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

    for (i = 0; i < NUM_CMD; i++)
    {
        if ( !(strcmp(cmd->cmd, cli_cmds[i])) )
        { return true; }
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
        memcpy(cmd_buf, cmd_start, cmd_end - cmd_start);
        cmd->p_input = NULL;
    }
    else
    {
        memcpy(cmd_buf, cmd_start, cmd_end - cmd_start);
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
            free(cmd->cmd); cmd->cmd = NULL;
        }
        if ( cmd->args )
        {
            free(cmd->args); cmd->args = NULL;
        }

        free(cmd);
        cmd = NULL;
    }
}


// =====================================================================================================================
// =====================================================================================================================


void error_reporter(const struct dc_error *err)
{
    fprintf(stderr, "ERROR: %s : %s : @ %zu : %d\n", err->file_name, err->function_name, err->line_number, 0);
    fprintf(stderr, "ERROR: %s\n", err->message);
}


void trace_reporter(__attribute__((unused)) const struct dc_posix_env *env,
                           const char *file_name, const char *function_name, size_t line_number)
{
    fprintf(stdout, "TRACE: %s : %s : @ %zu\n", file_name, function_name, line_number);
}
