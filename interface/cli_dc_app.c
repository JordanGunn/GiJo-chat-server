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
    settings->logout         = dc_setting_string_create(env, err);
    settings->send           = dc_setting_string_create(env, err);
    settings->get_users      = dc_setting_string_create(env, err);
    settings->create_channel = dc_setting_string_create(env, err);
    settings->join_channel   = dc_setting_string_create(env, err);
    settings->leave_channel  = dc_setting_string_create(env, err);


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
                    'I',
                    "LOGIN",
                    dc_string_from_string,
                    "LOGIN",
                    dc_string_from_config,
                    NULL},
            {(struct dc_setting *)settings->logout,
                    dc_options_set_string,
                    "LOGOUT",
                    required_argument,
                    'O',
                    "LOGOUT",
                    dc_string_from_string,
                    "LOGOUT",
                    dc_string_from_config,
                    NULL},
            {(struct dc_setting *)settings->send,
                    dc_options_set_string,
                    "SEND",
                    required_argument,
                    'S',
                    "SEND",
                    dc_string_from_string,
                    "SEND",
                    dc_string_from_config,
                    NULL},
            {(struct dc_setting *)settings->send,
                    dc_options_set_string,
                    "GET-USERS",
                    required_argument,
                    'U',
                    "GET-USERS",
                    dc_string_from_string,
                    "GET-USERS",
                    dc_string_from_config,
                    NULL},
            {(struct dc_setting *)settings->create_channel,
                    dc_options_set_string,
                    "CREATE-CHANNEL",
                    required_argument,
                    'C',
                    "CREATE-CHANNEL",
                    dc_string_from_string,
                    "CREATE-CHANNEL",
                    dc_string_from_config,
                    NULL},
            {(struct dc_setting *)settings->join_channel,
                    dc_options_set_string,
                    "JOIN-CHANNEL",
                    required_argument,
                    'J',
                    "JOIN-CHANNEL",
                    dc_string_from_string,
                    "JOIN-CHANNEL",
                    dc_string_from_config,
                    NULL},
            {(struct dc_setting *)settings->leave_channel,
                    dc_options_set_string,
                    "LEAVE-CHANNEL",
                    required_argument,
                    'L',
                    "LEAVE-CHANNEL",
                    dc_string_from_string,
                    "LEAVE-CHANNEL",
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
    settings->opts.flags = "I:O:S:C:J:L:h:p:";
    settings->opts.env_prefix = "CPT_CHAT_CLIENT_";

    return (struct dc_application_settings *)settings;
}

void chat_prompt();

int destroy_settings(const struct dc_posix_env *env,
                     __attribute__((unused)) struct dc_error *err,
                     struct dc_application_settings **psettings)
{
    struct application_settings *app_settings;

    DC_TRACE(env);
    app_settings = (struct application_settings *)*psettings;
    dc_setting_string_destroy(env, &app_settings->host          );
    dc_setting_string_destroy(env, &app_settings->port          );
    dc_setting_string_destroy(env, &app_settings->logout        );
    dc_setting_string_destroy(env, &app_settings->send          );
    dc_setting_string_destroy(env, &app_settings->get_users     );
    dc_setting_string_destroy(env, &app_settings->create_channel);
    dc_setting_string_destroy(env, &app_settings->join_channel  );
    dc_setting_string_destroy(env, &app_settings->leave_channel );

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
    char * user_input, * p_path;
    int pid, command_count, stat;
    char cwd_buf[SM_BUFF_SIZE] = {0};
    char * host, * port, * login, * logout, * send, * get_users;
    char * create_channel, * join_channel, * leave_channel;

    struct application_settings * app_settings;
    app_settings = (struct application_settings *) settings;

    getcwd(cwd_buf, SM_BUFF_SIZE);
    strncat(cwd_buf, APP_REL_PNAME, strlen(APP_REL_PNAME) + 1);
    p_path = strdup(cwd_buf);
    puts(p_path);

    host           = dc_setting_string_get(env, app_settings->host          );
    port           = dc_setting_string_get(env, app_settings->port          );
    login          = dc_setting_string_get(env, app_settings->login         );
    logout         = dc_setting_string_get(env, app_settings->logout        );
    send           = dc_setting_string_get(env, app_settings->send          );
    get_users      = dc_setting_string_get(env, app_settings->get_users     );
    create_channel = dc_setting_string_get(env, app_settings->create_channel);
    join_channel   = dc_setting_string_get(env, app_settings->join_channel  );
    leave_channel  = dc_setting_string_get(env, app_settings->leave_channel );

    if ( !user.LOGGED_IN )
    {
        login_handler(host, port, login);
    }

    char * commands[] =
    {
        send,
        logout,
        get_users,
        create_channel,
        join_channel,
        leave_channel
    };

    command_count = count_commands(commands);

    user_input = NULL;
    while ( user.LOGGED_IN )
    {
        if ( command_count == 0 )
        {

            while ( !is_valid_command(user_input) )
            {
                menu();
                user_input = get_user_input();
            }

            pid = fork();
            if ( pid == 0 )
            {
                execlp(p_path, APP_FNAME, "LOGIN", "jordan", "--LOGOUT", "jordan", (char *) NULL);
            }

            else
            {
                waitpid(pid, &stat, 0);
                free(user_input); user_input = NULL;
                if ( WIFEXITED(stat) ) { puts("success!"); }
            }

        }
        else
        {
            handle_commands(commands);
        }
    }

    close(user.client_info->fd);  // close the connection
    return EXIT_SUCCESS;
}


bool is_valid_command(char * input) {

    int i;
    char * start, * end;
    char cmd[SM_BUFF_SIZE] = {0};
    if ( !input ) { return false; }

    start = input;
    end = strchr(input, ' ');
    memcpy(cmd, start, end - start);

    for (i = 0; i < NUM_CMD; i++)
    {
        if ( !(strcmp(cmd, cli_commands[i])) )
        {
            return true;
        }
    }

    return false;
}


char *get_user_input()
{
    char buf[SM_BUFF_SIZE];

    read(STDIN_FILENO, buf, SM_BUFF_SIZE);
    return strdup(buf);
}


int count_commands(char * commands[NUM_CMD])
{
    int i, count;

    count = 0;
    for (i = 0; i < NUM_CMD; i++)
    {
        if ( commands[i] ) { count++; }
    }

    return count;
}


void menu()
{
    char menu_buf[XL_BUFF_SIZE] = {0};
    static char * logout, * get_users, * create_channel, * join_channel;
    static char * send, * title, * div, * leave_channel, * prompt;

    div = "=============================================================";
    title = "Choose from the following options...\n\n";
    send           = "  [0] --SEND <message>\n";
    get_users      = "  [1] --GET-USERS <message>\n";
    create_channel = "  [2] --CREATE-CHANNEL <id-1> <id-2> .. <id-n>\n";
    join_channel   = "  [3] --JOIN-CHANNEL <id>\n";
    leave_channel  = "  [4] --LEAVE-CHANNEL <id>\n";
    logout         = "  [5] --LOGOUT <name>\n";

    sprintf(menu_buf, "%s\n%s%s%s%s%s%s%s%s\n",
            div,
            title, send, get_users, create_channel,
            join_channel, leave_channel, logout,
            div
        );

    printf("%s", menu_buf);
    printf("\n");
    chat_prompt();
    fflush(stdout);
}


void chat_prompt()
{
    uint16_t channel;
    char * name, * prompt;
    name = strdup(user.name);
    channel = user.channel;

    printf("[ %s | (channel %hu) ] $ ", name, channel);
}


void login_handler(char * host, char * port, char * name)
{
    int fd;

    if ( name )
    {
        user.client_info = cpt_init_client_info(port, host);
        fd = tcp_init_client(host, port);
        user.client_info->fd = fd;

        if ( user_login(name) < 0 )
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


int user_login(char * name)
{
    int result;
    size_t res_size, req_size;

    CptResponse * res;
    uint8_t res_buf[LG_BUFF_SIZE] = {0};

    uint8_t req_buf[LG_BUFF_SIZE] = {0};

    // send data to server
    req_size = cpt_login(user.client_info, name, req_buf);
    result = tcp_client_send(user.client_info->fd, req_buf, req_size);
    user.client_info->channel = CHANNEL_ZERO;
    user.client_info->name = strdup(name);

    // collect echo from server
    if (result != -1)
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

void handle_commands(char * commands[NUM_CMD])
{
    if ( commands[LOGOUT] )
    {
        logout_handler();
    }

//    if ( commands[SEND] )
//    {
//        user_send();
//    }
//
//    if ( commands[GET_USERS] )
//    {
//        user_get_users();
//    }
//
//    if ( commands[CREATE_CHANNEL] )
//    {
//        user_create_channel();
//    }
//
//    if( commands[JOIN_CHANNEL] )
//    {
//        user_join_channel();
//    }
//
//    if ( commands[LEAVE_CHANNEL] )
//    {
//        user_leave_channel();
//    }
}


int user_logout()
{
    int result;
    size_t req_size;

    uint8_t req_buf[LG_BUFF_SIZE] = {0};

    // send data to server
    req_size = cpt_logout(user.client_info, req_buf);
    result = tcp_client_send(user.client_info->fd, req_buf, req_size);

    return result;
}


void logout_handler()
{
    user_logout();
    cpt_destroy_client_info(user.client_info);
    user.LOGGED_IN = false;
}


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
