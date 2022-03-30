//
// Created by jordan on 2022-02-10.
//
#include "cli_dc.h"

bool LOGGED_IN =  false;

pthread_mutex_t mutex;
int run(const struct dc_posix_env * env, struct dc_error * err, struct dc_application_settings *settings)
{

    UserState * ustate;
    pthread_t th[NUM_MSG_THREADS];
    char * host, * port, * login;

    struct application_settings * app_settings;
    app_settings = (struct application_settings *) settings;

    host  = dc_setting_string_get(env, app_settings->host );
    port  = dc_setting_string_get(env, app_settings->port );
    login = dc_setting_string_get(env, app_settings->login);


    ustate = user_state_init();
    if ( !LOGGED_IN )
    {

        user_login(ustate, host, port, login);
    }

    while ( LOGGED_IN )
    {
        ustate->cmd = cmd_init();

        chat_prompt(ustate);
        ustate->cmd->input = get_user_input();
        parse_cmd_input(ustate->cmd);
        thread_msgs(th, ustate);
        cmd_destroy(ustate->cmd);
    }

    close(ustate->client_info->fd);  // close the connection
    destroy_cli_user(ustate);
    return EXIT_SUCCESS;
}

void destroy_cli_user(UserState * ustate)
{

    if ( ustate->client_info )
    {
        cpt_destroy_client_info(ustate->client_info);
        ustate->client_info = NULL;
    }

}

void recv_handler(UserState * ustate)
{
    int on;
    char * block;
    ssize_t recv_size;
    CptResponse * res;
    uint8_t recv_buf[MD_BUFF_SIZE];

    on = 1;
    ioctl(ustate->client_info->fd, FIONBIO, (char *)&on);
    recv_size = tcp_client_recv(ustate->client_info->fd, recv_buf);
    on &= ~O_NONBLOCK;

    if ( recv_size != SYS_CALL_FAIL  )
    {
        res = cpt_parse_response(recv_buf, recv_size);
        if ( res )
        {
            //TODO SHMEM HERE!!!
            block = shmem_attach(FILENAME, BLOCK_SIZE);
            strncpy(block, (char *) res->data, BLOCK_SIZE);
            shmem_detach(block);
        }
        cpt_response_reset(res);
    }
}


void send_handler(UserState * ustate, char * msg)
{
    int send_res;
    char * block;
    size_t req_size;
    CptClientInfo * info;
    info = ustate->client_info;
    uint8_t req_buf[MD_BUFF_SIZE] = {0};

    req_size = cpt_send(info, req_buf, msg);
    send_res = tcp_client_send(
            info->fd, req_buf, req_size);

    if ( send_res > 0 )
    {
        block = shmem_attach(FILENAME, BLOCK_SIZE);
        strncpy(block, msg, BLOCK_SIZE);
        shmem_detach(block);
    }
    cpt_request_reset(info->packet);
}


void thread_msgs(pthread_t th[NUM_MSG_THREADS], UserState * ustate)
{
    int i;
    pthread_mutex_init(&mutex, NULL);
    for (i = 0; i< NUM_MSG_THREADS; i++)
    {
        if ( ((i % 2) == 0) )
        {
            if ((pthread_create(&th[i], NULL, &command_thread, ustate)) != 0 )
            {
                perror("Failed to create producer thread...");
            }
        }
        else
        {
            if ((pthread_create(&th[i], NULL, &send_thread, ustate)) != 0 )
            {
                perror("Failed to create consumer thread...");
            }
        }
    }

    for (i = 0; i < NUM_MSG_THREADS; i++)
    {
        if ( (pthread_join(th[i], NULL) != 0) )
        {
            perror("Failed to join threads...");
        }
    }
    pthread_mutex_destroy(&mutex);
}

UserState * user_state_init()
{
    UserState * ustate;

    ustate = malloc(sizeof (struct user_state));
    ustate->client_info = NULL;
    ustate->cmd = NULL;
    ustate->channel = 0;

    return ustate;
}


void user_login(UserState * ustate, char * host, char * port, char * name)
{
    int fd;

    if ( name )
    {
        ustate->client_info = cpt_init_client_info(port, host);
        fd = tcp_init_client(host, port);
        ustate->client_info->fd = fd;

        if (login_handler(ustate, name) < 0 )
        {
            printf("Failed to login to chat...\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            LOGGED_IN = true;
            ustate->client_info->name = strdup(name);
            ustate->channel = CHANNEL_ZERO;
        }
    }
    else
    {
        printf("User not logged in!\n");
        exit(EXIT_FAILURE);
    }
}



int login_handler(UserState * ustate, char * name)
{
    int result;
    CptResponse * res;
    size_t res_size, req_size;
    uint8_t res_buf[LG_BUFF_SIZE] = {0};
    uint8_t req_buf[LG_BUFF_SIZE] = {0};

    // send data to server
    res_size = 0; req_size = 0;
    req_size = cpt_login(ustate->client_info, req_buf, name);
    result = tcp_client_send(ustate->client_info->fd, req_buf, req_size);
    ustate->client_info->channel = CHANNEL_ZERO;
    ustate->client_info->name = strdup(name);

    if (result != SYS_CALL_FAIL)
    {
        res_size = tcp_client_recv(ustate->client_info->fd, res_buf);
        res = cpt_parse_response(res_buf, res_size);
        if ( res )
        {
            if (res->code == SUCCESS)
            {
                printf("\n\nLogged in to chat server as %s@%s:%s\n\n",
                       name, ustate->client_info->ip, ustate->client_info->port);
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


void logout_handler(UserState * ustate)
{
    size_t req_size;
    uint8_t req_buf[LG_BUFF_SIZE] = {0};

    req_size = cpt_logout(ustate->client_info, req_buf);
    tcp_client_send(ustate->client_info->fd, req_buf, req_size);
    LOGGED_IN = false;
}


void create_channel_handler(UserState * ustate)
{
    int result;
    uint16_t new_cid;
    CptResponse * res;
    size_t req_size, res_size;
    uint8_t req_buf[MD_BUFF_SIZE] = {0};
    uint8_t res_buf[MD_BUFF_SIZE] = {0};

    res_size = 0; req_size = 0;
    req_size = cpt_create_channel(
            ustate->client_info, req_buf, (char *) ustate->cmd->args);

    result = tcp_client_send(
            ustate->client_info->fd, req_buf, req_size);

    if ( result != SYS_CALL_FAIL )
    {
        res_size = tcp_client_recv(
                ustate->client_info->fd, res_buf);

        res = cpt_parse_response(res_buf, res_size);
        if ( res->code == SUCCESS )
        {
            new_cid = (uint16_t) ( *(res->data) ); // new channel id is in response
            ustate->channel = new_cid;
            push_data(ustate->client_info->channels, &new_cid, sizeof(uint16_t));
            printf("\nSuccessfully created channel with users: [ %s ]\n",
                   (char *)ustate->cmd->args);
        } else { printf("Failed to create channel with code: %d", res->code); }
    }
}


void get_users_handler(UserState * ustate)
{
    int result;
    char * args_end;
    CptResponse * res;
    uint16_t channel_id;
    CptClientInfo * info;
    size_t req_size, res_size;
    uint8_t req_buf[MD_BUFF_SIZE] = {0};
    uint8_t res_buf[LG_BUFF_SIZE] = {0};

    info = ustate->client_info;
    res_size = 0; req_size = 0;
    channel_id = ( ustate->cmd->args )
    ? (uint16_t) strtol( ustate->cmd->args, &args_end, 10)
    : ustate->channel;

    req_size = cpt_get_users(
            info, req_buf, channel_id);

    result = tcp_client_send(
            info->fd, req_buf, req_size);

    if ( result != SYS_CALL_FAIL )
    {
        res_size = tcp_client_recv(
                info->fd, res_buf);

        res = cpt_parse_response(res_buf, res_size);
        if ( res->code == SUCCESS )
        {
            printf("%s\n", (char *)res->data);
            cpt_response_reset(res);
        } else { printf("Failed to get users with code: %d\n", res->code); }
    }

}


void leave_channel_handler(UserState * ustate)
{
    int result;
    CptResponse * res;
    size_t req_size, res_size;
    uint8_t req_buf[MD_BUFF_SIZE] = {0};
    uint8_t res_buf[LG_BUFF_SIZE] = {0};

    req_size = 0, res_size = 0;
    req_size = cpt_leave_channel(
            ustate->client_info, req_buf, ustate->channel);

    result = tcp_client_send(
            ustate->client_info->fd, req_buf, req_size);

    if ( result != SYS_CALL_FAIL )
    {
        res_size = tcp_client_recv(
                ustate->client_info->fd, res_buf);

        res = cpt_parse_response(res_buf, res_size);
        if ( res->code == SUCCESS )
        {
            printf("%s\n", (char *)res->data);
            ustate->channel = CHANNEL_ZERO;
        } else { printf("Failed to leave channel with code: %d\n", res->code); }
    }
}



void join_channel_handler(UserState * ustate) {

    int result;
    uint16_t cid;
    char * args_end;
    CptResponse * res;
    uint16_t channel_id;
    size_t req_size, res_size;
    uint8_t req_buf[MD_BUFF_SIZE] = {0};
    uint8_t res_buf[LG_BUFF_SIZE] = {0};

    req_size = 0; res_size = 0;
    channel_id = (uint16_t) strtol(ustate->cmd->args, &args_end, 10);

    req_size = cpt_join_channel(
            ustate->client_info, req_buf, channel_id);

    result = tcp_client_send(
            ustate->client_info->fd, req_buf, req_size);

    if ( result != SYS_CALL_FAIL )
    {
        res_size = tcp_client_recv(
                ustate->client_info->fd, res_buf);

        res = cpt_parse_response(res_buf, res_size);
        if ( res->code == SUCCESS )
        {
            cid = (uint16_t) ( *(res->data) );
            ustate->channel = cid;

        } else { printf("Failed to join channel with code: %d\n", res->code); }
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


void chat_prompt(UserState * ustate)
{
    uint16_t channel;
    char * name;
    name = strdup(ustate->client_info->name);
    channel = ustate->channel;

    printf("[ %s | (channel %hu) ] $ ", name, channel);
    fflush(stdout);

    free(name); name = NULL;
}


char * get_user_input()
{
    char buf[SM_BUFF_SIZE] = {0};

    read(STDIN_FILENO, buf, SM_BUFF_SIZE);
    return strdup(buf);
}


void * send_thread(void * data)
{
    UserState * ustate;

    ustate = (UserState *) data;

    pthread_mutex_lock(&mutex);
    if (!is_valid_cmd(ustate->cmd))
    {
        send_handler(ustate,ustate->cmd->input);
    }

    pthread_mutex_unlock(&mutex);

    return (void *) ustate->cmd;
}


void * command_thread(void * data)
{
    UserState * ustate;
    ustate = (UserState * ) data;

    pthread_mutex_lock(&mutex);
    if ( is_cmd(ustate->cmd, cli_cmds[MENU]           )) { menu();                      }
    else if ( is_cmd(ustate->cmd, cli_cmds[LOGOUT]         )) { logout_handler(ustate);            }
    else if ( is_cmd(ustate->cmd, cli_cmds[GET_USERS]      )) { get_users_handler(ustate);      }
    else if ( is_cmd(ustate->cmd, cli_cmds[CREATE_CHANNEL] )) { create_channel_handler(ustate); }
    else if ( is_cmd(ustate->cmd, cli_cmds[JOIN_CHANNEL]   )) { join_channel_handler(ustate);   }
    else if ( is_cmd(ustate->cmd, cli_cmds[LEAVE_CHANNEL]  )) { leave_channel_handler(ustate);     }
    else { recv_handler(ustate); }
    pthread_mutex_unlock(&mutex);

    return (void *) ustate;
}


// =====================================================================================================================
// =====================================================================================================================



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

void error_reporter(const struct dc_error *err)
{
    fprintf(stderr, "ERROR: %s : %s : @ %zu : %d\n", err->file_name, err->function_name, err->line_number, 0);
    fprintf(stderr, "ERROR: %s\n", err->message);
}
