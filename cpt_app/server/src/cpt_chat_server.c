//
// Created by jordan on 2022-03-08.
//

#include "cpt_chat_server.h"

static int GCFD;
static int nfds;
static struct pollfd poll_fds[MAX_SERVER_FDS];

static int result, active_nfds, i, j;
static bool is_fatal_error, close_conn, compress_array;


int main(void)
{
    run();
    return 0;
}

void run(void)
{
    // ===============================================
    //    I N I T I A L    S E T U P
    // ===============================================
    ServerInfo * info;
    Channel * gc; Channels * dir;

    gc = init_global_channel();
    dir = init_channel_directory(gc);
    info = cpt_server_info_init(gc, dir);

    nfds = 1;
    close_conn = false;
    is_fatal_error = false;

    // ------------------------------------------------

    /* Setup listener socket. */
    listen_socket_init( gc );

    // **************************************************
    // ************ E V E N T ** L O O P ****************   AMERICA !!!!!!!!!!!!!!!
    // **************************************************
    do {
        printf("Waiting on poll...\n");
        result = poll(poll_fds, (nfds_t) nfds, POLL_TIMEOUT_5M);

        if ( should_end_event_loop(result) ) { break; }

        active_nfds = nfds; /* Number of fds with events ready */
        for (i = 0; i < active_nfds; i++)
        {
            info->current_id = poll_fds[i].fd;
            /* -------------------------------------------------- */
            /* No event on current socket, go back to top of loop */
            /* -------------------------------------------------- */
            if ( poll_fds[i].revents == 0 ) { continue; }


            // ==============================================================================================

            /* ------------------------------------ */
            /* Unexpected event causing fatal error */
            /* ------------------------------------ */
            if ( !(is_revent_POLLIN(i)) )
            {
                printf("Unexpected event: %d\n", poll_fds[i].revents);
                is_fatal_error = true; break;
            }

            // ==============================================================================================

            /* ------------------------------------------------ */
            /* Event on listener socket, new connection arrived */
            /* ------------------------------------------------ */
            if ( poll_fds[i].fd == GCFD )
            {
                printf("  New connections found...\n");
                printf("Checking queued login attempts...\n");
                do
                { /* Check accept backlog for login attempts */
                    result = login_event(info);
                    if ( result == SUCCESS )
                    {
                        poll_fds[nfds].fd = info->current_id;
                        poll_fds[nfds].events = POLLIN;
                        nfds++;
                    }
                } while ( result != FAILURE );
            }

            // ==============================================================================================

            /* ----------------------------------------------- */
            /* handle all events from existing connected users */
            /* ----------------------------------------------- */
            else
            {
                do {
                    if ( is_revent_POLLIN(i) )
                    {   /* Receive incoming data from sockets */
                        close_conn = false; // !
                        info->current_id = poll_fds[i].fd;

                        CptRequest * req;
                        ssize_t req_size;
                        uint8_t req_buf[LG_BUFF_SIZE] = {0};
                        req_size = tcp_server_recv(info->current_id, req_buf);
                        req = cpt_parse_request(req_buf, (size_t) req_size);

                        if ( req->command == LOGOUT )
                        {
                            logout_event(info);
                            cpt_request_destroy(req);
                            close_conn = true; break;

                        } else { handle_event(info, req); }

                        if ( req_size < 0 )
                        {
                            if ( errno != EWOULDBLOCK )
                            {
                                perror("  recv() failed...");
                                close_conn = true; // !
                            } break;
                        }

                        if (req_size == 0 )
                        {
                            printf("  Connection closed\n");
                            close_conn = true; break;
                        }
                        break;
                    }
                } while ( true );

                if ( close_conn )
                { /* handle close connection flag if set */
                    close(poll_fds[i].fd);
                    poll_fds[i].fd = -1; // setting fd to -1 will cause poll() to ignore this fd index
                    compress_array = true;
                }
            }
        }

        if ( compress_array )
        { compress_fds(); }

    } while ( !is_fatal_error );

    cpt_server_info_destroy(info);
}

void compress_fds()
{
    compress_array = false;
    for (i = 0; i < nfds; i++)
    {
        if ( poll_fds[i].fd == -1 )
        {
            for ( j = i; j < nfds; j++)
            {
                poll_fds[j].fd = poll_fds[j + 1].fd;
            }
            i--; nfds--;
        }
    }
}


void join_channel_event(ServerInfo *info, uint16_t channel_id)
{
    char * msg;
    int join_res;
    size_t res_size;
    CptResponse * res;
    uint8_t res_buf[MD_BUFF_SIZE] = {0};

    join_res = cpt_join_channel_response(info, channel_id);
    res = cpt_response_init();
    if ( join_res == SUCCESS )
    { /* Send back confirmation if successful */
        res->code = (uint8_t) JOIN_CHANNEL;
        res->data_size = sizeof(channel_id);
        res->data = (uint8_t *) &channel_id;
    }
    else
    {
        msg = "Failed to join channel";
        res->code = (uint8_t) FAILURE;
        res->data_size = (uint16_t) strlen(msg);
        res->data = (uint8_t *) msg;
    }

    res_size = cpt_serialize_response(res, res_buf);
    tcp_server_send(info->current_id, res_buf, res_size);
    cpt_response_destroy(res);
}


void leave_channel_event(ServerInfo * info, uint16_t channel_id)
{
    int lc_res;
    char * res_msg;
    size_t res_size;
    CptResponse * res;
    uint8_t res_buf[MD_BUFF_SIZE] = {0};
    char res_msg_buf[SM_BUFF_SIZE] = {0};

    lc_res = cpt_leave_channel_response(info, channel_id);
    res = cpt_response_init();
    if ( lc_res == SUCCESS )
    { /* Send back confirmation if successful */
        sprintf(res_msg_buf,
            "Successfully left channel %d", channel_id);
        res_msg = strdup(res_msg_buf);
        res->code = LEAVE_CHANNEL;
    }
    else
    {
        res_msg = strdup("Failed to leave channel");
        res->code = FAILURE;
    }

    res->data_size = (uint16_t) strlen(res_msg);
    res->data = (uint8_t *) res_msg;
    res_size = cpt_serialize_response(res, res_buf);
    tcp_server_send(info->current_id, res_buf, res_size);
    cpt_response_destroy(res);
}


int login_event(ServerInfo * info)
{
    char * msg;
    CptRequest * req;
    CptResponse * res;
    int login_res, new_fd;
    size_t res_size, req_size;
    uint8_t res_buf[MD_BUFF_SIZE] = {0};
    uint8_t req_buf[MD_BUFF_SIZE] = {0};

    if ( ((new_fd = handle_new_accept()) != SYS_CALL_FAIL) )
    {   /* If accept system call succeeds, attempt to add user */
        res = cpt_response_init();
        info->current_id = new_fd;
        req_size = (size_t) tcp_server_recv(new_fd, req_buf);
        req = cpt_parse_request(req_buf, req_size);
        login_res = cpt_login_response(info, (char *) req->msg);

        if (login_res == SUCCESS)
        { /* If login succeeded, add file desc and set the events */
            msg = strdup("Success!");
            res->code = (uint8_t) LOGIN;
        }
        else
        {
            msg = strdup("Failure");
            res->code = (uint8_t) FAILURE;
        }
        res->data_size = (uint16_t) strlen(msg);
        res->data = (uint8_t *) msg;

        res_size = cpt_serialize_response(res, res_buf);
        tcp_server_send(new_fd, res_buf, res_size);
        cpt_response_destroy(res);
        cpt_request_destroy(req);
    }
    else
    {
        if (errno != EWOULDBLOCK)
        {
            perror("  accept() failed");
            is_fatal_error = true;
        }
    }

    return (new_fd != SYS_CALL_FAIL) ? SUCCESS : FAILURE;
}


void logout_event(ServerInfo * info)
{
    int lo_res;
    lo_res = cpt_logout_response(info);

    if ( lo_res == SUCCESS )
    {
        printf("  User with ID %d logged out...\n", info->current_id);
    }
}


void create_channel_event(ServerInfo * info, char * id_list)
{
    char * msg;
    int cc_res;
    uint16_t ncid;
    size_t res_size;
    CptResponse * res;
    uint8_t res_buf[MD_BUFF_SIZE] = {0};

    cc_res = cpt_create_channel_response(info, id_list);
    res = cpt_response_init();

    ncid = ((uint16_t) (info->dir->length - 1));

    if ( cc_res == SUCCESS )
    {
        res->data = (uint8_t *) &ncid;
        res->data_size = sizeof(ncid);
        res->code = (uint8_t) CREATE_CHANNEL;
    }
    else
    {
        msg = strdup("Failed to create channel...");
        res->data = (uint8_t *) msg;
        res->data_size = sizeof(msg);
        res->code = (uint8_t) FAILURE;
    }

    res_size = cpt_serialize_response(res, res_buf);
    tcp_server_send(info->current_id, res_buf, res_size);
    cpt_response_destroy(res);
}


void get_users_event(ServerInfo * info, int chan_id)
{
    int gu_res;
    size_t res_size;
    uint8_t res_buf[LG_BUFF_SIZE] = {0};

    info->res = cpt_response_init();
    gu_res = cpt_get_users_response(info, (uint16_t) chan_id);

    if ( gu_res == SUCCESS )
    {
        printf("GET_USERS event occurred successfully...\n");
    }

    res_size = cpt_serialize_response(info->res, res_buf);
    tcp_server_send(info->current_id, res_buf, res_size);
    cpt_response_destroy(info->res);
}


void send_event(ServerInfo * info, char * msg, int channel_id)
{
    User * user;
    size_t res_size;
    uint16_t dest_id;
    Channel * channel;
    UserNode * user_node;
    uint8_t res_buf[LG_BUFF_SIZE] = {0};

    channel = find_channel(info->dir, (uint16_t) channel_id);
    if ( channel )
    {
        user_node = get_head_user(channel->users);
        if ( user_node )
        {
            info->res = cpt_response_init();
            info->res->code = (uint8_t) SEND;
            info->res->data_size = (uint16_t) strlen(msg);
            info->res->data = (uint8_t *) strdup(msg);
            res_size = cpt_serialize_response(info->res, res_buf);

            while ( user_node )
            {
                user = user_node->user;
                dest_id = user->id;
                if ( dest_id != GC_ROOT_USR_ID )
                {
                    if ( dest_id != info->current_id )
                    {
                        tcp_server_send(dest_id, res_buf, res_size);
                    }
                }
                user_node = user_node->next_user;
            }
        }
    }
    cpt_response_destroy(info->res);
}


bool is_revent_POLLIN(int index)
{
    return (
        poll_fds[index].revents ==
        ( ((uint16_t) poll_fds[index].revents) & POLLIN )
    );
}


int handle_new_accept(void)
{
    int new_fd;
    struct sockaddr_storage client_addr;
    new_fd = tcp_server_accept(&client_addr, poll_fds[CHANNEL_ZERO].fd);
    if ( new_fd < 0 ) /* Accept will fail safely with EWOULDBLOCK */
    {
        if (errno != EWOULDBLOCK) /* if errno is not EWOULDBLOCK, fatal error occurred */
        {
            perror("  accept()");
        }
    }
    return new_fd;
}


void listen_socket_init(Channel * gc)
{
    memset(poll_fds, 0 , sizeof(poll_fds));
    if ((GCFD = tcp_server_init(IP_LOCAL_LB, PORT_8080)) != SYS_CALL_FAIL)
    {
        gc->fd = GCFD;
        poll_fds[CHANNEL_ZERO].fd = gc->fd;
        poll_fds[CHANNEL_ZERO].events = POLLIN;
    }
    else { exit( SERV_FATAL ); }
}


bool should_end_event_loop(int poll_result)
{
    bool end_event_loop;

    end_event_loop = false;
    if ( poll_result < 0 ) /* check if poll() failed */
    {
        perror("poll()");
        end_event_loop = true;
    }

    if ( poll_result == 0 ) /* poll returns 0 if it timed out */
    {
        printf("Poll timed out...");
        end_event_loop = true;
    }

    return end_event_loop;
}


void handle_event(ServerInfo * info, CptRequest * req)
{

    if ( req->command == SEND )
    {
    puts("  SEND event");
    send_event(info, (char *) req->msg, req->channel_id);
    cpt_request_destroy(req);
    }

    if ( req->command == GET_USERS )
    {
    puts("  GET_USERS event");
    get_users_event(info, req->channel_id);
    cpt_request_destroy(req);
    }

    if ( req->command == CREATE_CHANNEL )
    {
    puts("  CREATE_CHANNEL event");
    create_channel_event(info, (char *) req->msg);
    cpt_request_destroy(req);
    }

    if ( req->command == LEAVE_CHANNEL )
    {
    puts("  LEAVE_CHANNEL event");
    leave_channel_event(info, req->channel_id);
    cpt_request_destroy(req);
    }

    if ( req->command == JOIN_CHANNEL )
    {
    puts("  JOIN_CHANNEL event");
    join_channel_event(info, req->channel_id);
    cpt_request_destroy(req);
    }
}
