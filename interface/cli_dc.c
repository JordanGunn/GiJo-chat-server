//
// Created by jordan on 2022-02-10.
//
#include "cli_dc.h"


pthread_mutex_t mutex;
pthread_cond_t receiving;
bool is_receiving = false;
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
    if ( !ustate->LOGGED_IN )
    {
        user_login(ustate, host, port, login);
    }


    thread_chat_io(th, ustate);


    close(ustate->client_info->fd);  // close the connection
    user_state_destroy(ustate);
    return EXIT_SUCCESS;
}


void thread_chat_io(pthread_t th[NUM_MSG_THREADS], UserState * ustate)
{
    int i;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&receiving, NULL);
    for (i = 0; i< NUM_MSG_THREADS; i++)
    {
        if ( ((i % 2) == 0) )
        {
            if ((pthread_create(&th[i], NULL, &send_thread, ustate)) != 0 )
            {
                perror("Failed to create consumer thread...");
            }
        }
        else
        {
            if ((pthread_create(&th[i], NULL, &recv_thread, ustate)) != 0 )
            {
                perror("Failed to create producer thread...");
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
    pthread_cond_destroy(&receiving);
}

// ===========================================================

void * send_thread(void * user_state)
{
    UserState * ustate;

    ustate = (UserState *) user_state;


    while ( ustate->LOGGED_IN )
    {

        command_handler(ustate);
    }




    return (void *) ustate;
}


void command_handler(UserState * ustate)
{

    ustate->cmd = cmd_init();
    chat_prompt(ustate);

    ustate->cmd->input = get_user_input();
    parse_cmd_input(ustate->cmd);

    if ( ustate->cmd )
    {
        if (is_valid_cmd(ustate->cmd))
        {
            if ( is_cmd(ustate->cmd, cli_cmds[MENU_CMD]           )) { menu();                         }
            if ( is_cmd(ustate->cmd, cli_cmds[LOGOUT_CMD]         )) { logout_handler(ustate);         }
            if ( is_cmd(ustate->cmd, cli_cmds[GET_USERS_CMD]      )) { get_users_handler(ustate);      }
            if ( is_cmd(ustate->cmd, cli_cmds[CREATE_CHANNEL_CMD] )) { create_channel_handler(ustate); }
            if ( is_cmd(ustate->cmd, cli_cmds[JOIN_CHANNEL_CMD]   )) { join_channel_handler(ustate);   }
            if ( is_cmd(ustate->cmd, cli_cmds[LEAVE_CHANNEL_CMD]  )) { leave_channel_handler(ustate);  }
        }
        else
        {
            if ( ustate->cmd->input )
            {
                if ( strlen(ustate->cmd->input) )
                {
                    send_handler(ustate);
                }
            }
        }

        is_receiving = true;
        pthread_mutex_lock(&mutex);
        while ( is_receiving )
        {
            printf("Waiting for response...\n");
            pthread_cond_wait(&receiving, &mutex);
        }
        pthread_mutex_unlock(&mutex);

//        cmd_destroy(ustate->cmd);
    }
    cmd_destroy(ustate->cmd);
}

// ===========================================================================

void * recv_thread(void * user_state)
{
    UserState * ustate;

    ustate = (UserState *) user_state;

    while ( ustate->LOGGED_IN )
    {
        recv_handler(ustate);
    }

    return (void *) ustate;
}


void recv_handler(UserState * ustate)
{

    char * block;
    CptResponse * res;
    int cid;
    uint8_t res_buf[LG_BUFF_SIZE] = {0};
    ssize_t res_size;

    do {
        res_size = tcp_client_recv(ustate->client_info->fd, res_buf);

        if (res_size >= 0)
            { is_receiving = true; }
        else
            { is_receiving = false; }

    } while ( res_size <= 0 );

    // =============================
    pthread_mutex_lock(&mutex);
    // ==============================

    if (res_size > 0)
    {
        res = cpt_parse_response(res_buf, res_size);
        if ( res )
        {
            if ( res->code == (uint8_t) GET_USERS )
            {
                printf("%s\n", (char *)res->data);
            }

            if ( res->code == (uint8_t) CREATE_CHANNEL )
            {
                cid = (uint16_t) ( *(res->data) ); // new channel id is in response
                ustate->channel = cid;
                printf("\nSuccessfully created channel %d\n", cid);
            }

            if ( res->code == (uint8_t) JOIN_CHANNEL )
            {
                cid = (uint16_t) ( *(res->data) );
                ustate->channel = cid;
            }

            if ( res->code == (uint8_t) LEAVE_CHANNEL )
            {
                printf("%s\n", (char *)res->data);
                ustate->channel = CHANNEL_ZERO;
            }

            if ( res->code == (uint8_t) SEND )
            {
                block = shmem_attach(FILENAME, BLOCK_SIZE);
                strncpy(block, (char *) res->data, BLOCK_SIZE);
                shmem_detach(block);
            }
            cpt_response_reset(res);
        }
    }
    // ===================================
    is_receiving = false;
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&receiving);
    // ===================================
}

// ========================================================================


void user_login(UserState * ustate, char * host, char * port, char * name)
{
    int fd, on;

    on = 1;
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
            ustate->LOGGED_IN = true;
            ustate->client_info->name = strdup(name);
            ustate->channel = CHANNEL_ZERO;
            ioctl(ustate->client_info->fd, FIONBIO, (char *)&on);
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
    static char * title, * div, * leave_channel, * menu;

    div = "==================================================";
    title = "Choose from the following options...\n\n";
    get_users      = "  [1] @get-users <chan_id>\n";
    create_channel = "  [2] @create-channel \"<uid-1> <uid-2>.. <uid-n>\"\n";
    join_channel   = "  [3] @join-channel <chan_id>\n";
    leave_channel  = "  [4] @leave-channel <chan_id>\n";
    logout         = "  [5] @logout <name>\n";
    menu           = "  [6] @menu\n";

    sprintf(menu_buf, "%s\n%s%s%s%s%s%s%s%s\n",
            div,
            title, get_users, create_channel,
            join_channel, leave_channel, logout, menu,
            div
    );

    printf("%s", menu_buf);
    printf("\n");
    fflush(stdout);
}


char * get_user_input()
{
    char buf[SM_BUFF_SIZE] = {0};

    read(STDIN_FILENO, buf, SM_BUFF_SIZE);
    return strdup(buf);
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


// =====================================================================================================================
//  D C   A P P L I C A T I O N    F R A M E W O R K    F U N C T I O N S
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
