//
// Created by jordan on 2022-02-10.
//
#include <signal.h>
#include "cli_dc.h"


/* -- Global threading elements -- */
pthread_mutex_t     mutex;
pthread_cond_t      receiving;
bool is_receiving = false;
/* -------------------------------- */

int run(const struct dc_posix_env * env, struct dc_error * err, struct dc_application_settings *settings)
{
    UserState * ustate;
    char * host, * port, * login;
    pthread_t th[NUM_MSG_THREADS];

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

    if ( ((ustate->pid = fork()) != SYS_CALL_FAIL) )
    {
        if ( ustate->pid > 0 ) /* parent */
        {
            kill(ustate->pid, SIGSTOP);
            thread_chat_io(th, ustate);
        }
        else /* child */
        {
            run_voice_chat("127.0.0.1", "8080"); // TODO remove hardcoded IP and PORT
        }
    }

    /* clean up before exiting */
    close(ustate->client_info->fd);
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


void * send_thread(void * user_state)
{
    UserState * ustate;

    ustate = (UserState *) user_state;
    while ( ustate->LOGGED_IN )
    {

        ustate->cmd = cmd_init();
        prompt(ustate);

        ustate->cmd->input = cmd_get_input();
        parse_cmd_input(ustate->cmd);

        if ( ustate->cmd )
        {
            pthread_mutex_lock(&mutex);
            if ( is_valid_cmd(ustate->cmd) )
            {
                is_receiving = true;
                cmd_handler(ustate);

                ( is_voice_chan(ustate) )
                    ? kill(ustate->pid, SIGCONT)
                    : kill(ustate->pid, SIGSTOP);

                if ( is_cmd(ustate->cmd, cli_cmds[LOGOUT_CMD]) )
                { is_receiving = false; }

                while(is_receiving)
                {
                    puts("Waiting for response...");
                    pthread_cond_wait(&receiving, &mutex);
                }
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
            pthread_mutex_unlock(&mutex);
        }
        cmd_destroy(ustate->cmd);
        ustate->cmd = NULL;
    }

    return (void *) ustate;
}


void * recv_thread(void * user_state)
{
    ssize_t res_size;
    CptResponse * res;
    UserState * ustate;
    uint8_t res_buf[LG_BUFF_SIZE] = {0};

    ustate = (UserState *) user_state;

    while (ustate->LOGGED_IN)
    {
        res_size = tcp_client_recv(
                ustate->client_info->fd, res_buf);

        if (res_size > 0)
        {
            pthread_mutex_lock(&mutex);

            res = cpt_parse_response(res_buf);
            if ( res )
            {
                recv_handler(ustate, res);

                is_receiving = false;
                pthread_mutex_unlock(&mutex);
                pthread_cond_signal(&receiving);

                cpt_response_reset(res);

            } else { pthread_mutex_unlock(&mutex); }

        }
    }
    return (void *) ustate;
}



void user_login(UserState * ustate, char * host, char * port, char * name)
{
    int fd, on;

    on = 1;
    if ( name )
    {
        ustate->client_info = cpt_init_client_info(port, host);
        fd = tcp_init_client(host, port);
        ustate->client_info->fd = fd;

        if ( login_handler(ustate, name) < 0 )
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


void menu(void)
{
    is_receiving = false;
    char menu_buf[XL_BUFF_SIZE] = {0};
    static char * logout, * get_users, * create_channel, * join_channel, * create_vchannel;
    static char * title, * div, * leave_channel, * menu;

    div = "==================================================";
    title = "Choose from the following options...\n\n";
    get_users       = "  [1] @get-users <chan_id>\n";
    create_channel  = "  [2] @create-channel \"<uid-1> <uid-2>.. <uid-n>\"\n";
    create_vchannel = "  [3] @create-vchannel \"<uid-1> <uid-2>.. <uid-n>\"\n";
    join_channel    = "  [4] @join-channel <chan_id>\n";
    leave_channel   = "  [5] @leave-channel <chan_id>\n";
    logout          = "  [6] @logout <name>\n";
    menu            = "  [7] @menu\n";

    sprintf(menu_buf, "%s\n%s%s%s%s%s%s%s%s%s\n",
            div,
            title, get_users, create_channel, create_vchannel,
            join_channel, leave_channel, logout, menu,
            div
    );

    printf("%s", menu_buf);
    printf("\n");
    fflush(stdout);
}


void prompt(UserState * ustate)
{
    uint16_t channel;
    char * name;
    name = strdup(ustate->client_info->name);
    channel = ustate->channel;

    printf("[ %s | (channel %hu) ] $ ", name, channel);
    fflush(stdout);

    free(name); name = NULL;
}
