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

void run()
{
    // ===============================================
    //    I N I T I A L    S E T U P
    // ===============================================
    CptServerInfo * server_info;
    Channel * gc; Channels * dir;

    gc = init_global_channel();
    dir = init_channel_directory(gc);
    server_info = cpt_server_info_init(gc, dir);

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
        result = poll(poll_fds, nfds, POLL_TIMEOUT_5M);

        if ( should_end_event_loop(result) ) { break; }

        active_nfds = nfds; /* Number of fds with events ready */
        for (i = 0; i < active_nfds; i++)
        {
            server_info->current_id = poll_fds[i].fd;
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
                    result = login_event( server_info );
                } while ( result != FAILURE );
            }

            // ==============================================================================================

            /* ----------------------------------------------- */
            /* handle all events from existing connected users */
            /* ----------------------------------------------- */
            else
            {
                int id;
                ssize_t req_size;
                uint8_t req_buf[LG_BUFF_SIZE] = {0};
                close_conn = false; // !
                id = poll_fds[i].fd;

                do
                {
                    if ( is_revent_POLLIN(i) )
                    {   /* Receive incoming data from sockets */

                        CptRequest * req;
                        server_info->current_id = id;
                        req_size = tcp_server_recv(id, req_buf);
                        req = cpt_parse_request(req_buf, req_size);

                        if ( req->command == LOGOUT )
                        {
                            puts("  LOGOUT event");
                            logout_event(server_info);
                            cpt_request_destroy(req);
                            close_conn = true; break;
                        }

                        if ( req->command == SEND )
                        {
                            puts("  SEND event");
                        }

                        if ( req->command == GET_USERS )
                        {
                            puts("  GET_USERS event");
                            get_users_event(server_info, req->channel_id);
                            cpt_request_destroy(req);
                        }

                        if ( req->command == CREATE_CHANNEL )
                        {
                            puts("  CREATE_CHANNEL event");
                            create_channel_event(server_info, (char *) req->msg);
                            cpt_request_destroy(req);
                        }

                        if ( req->command == LEAVE_CHANNEL )
                        {
                            puts("  LEAVE_CHANNEL event");
                            leave_channel_event(server_info, req->channel_id);
                            cpt_request_destroy(req);
                        }

                        if ( req->command == JOIN_CHANNEL )
                        {
                            puts("  JOIN_CHANNEL event");
                            join_channel_event(server_info, req->channel_id);
                            cpt_request_destroy(req);
                        }

                        if ( req_size < 0 )
                        { /* Check if error is not EWOULDBLOCK */
                            if ( errno != EWOULDBLOCK )
                            {
                                perror("  recv() failed...");
                                close_conn = true; // !
                            }
                            break;
                        }

                        if (req_size == 0 )
                        { /* Check if connection closed by client */
                            printf("  Connection closed\n");
                            close_conn = true;
                            break;
                        }

                        break;
                    }
                //TODO Make sure the loop-breaking condition below doesn't completely break everything
                } while ( true );

                if ( close_conn )
                { /* handle close connection flag if set */
                    close(poll_fds[i].fd);
                    poll_fds[i].fd = -1; // setting fd to -1 will cause poll() to ignore this fd index
                    compress_array = true;
                }
            } /* CURRENT USER CONNECTION EVENTS */

        } /* POLL LOOP FOR ALL USER CONNECTIONS */

        if ( compress_array )
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
    } while ( !is_fatal_error );

    cpt_server_info_destroy(server_info);  /* Close existing sockets before ending */
}

void join_channel_event(CptServerInfo *info, uint16_t channel_id)
{
    char * msg;
    int join_res;
    size_t res_size;
    CptResponse * res;
    uint8_t res_buf[MD_BUFF_SIZE] = {0};

    join_res = cpt_join_channel_response(info, channel_id);
    res = cpt_response_init(join_res);
    res->code = join_res;
    if ( res->code == SUCCESS )
    { /* Send back confirmation if successful */
        res->data = (uint8_t *) &channel_id;
    }
    else
    {
        msg = "Failed to join channel";
        res->data = (uint8_t *) msg;
    }

    res_size = cpt_serialize_response(res, res_buf);
    tcp_server_send(info->current_id, res_buf, res_size);
    cpt_response_destroy(res);
}

void leave_channel_event(CptServerInfo * info, uint16_t channel_id)
{
    int lc_res;
    size_t res_size;
    CptResponse * res;
    uint8_t res_buf[MD_BUFF_SIZE] = {0};
    char res_msg_buf[SM_BUFF_SIZE] = {0};

    lc_res = cpt_leave_channel_response(info, channel_id);
    res = cpt_response_init(lc_res);
    res->code = lc_res;
    if ( res->code == SUCCESS )
    { /* Send back confirmation if successful */
        sprintf(res_msg_buf,
                "Successfully left channel %d", channel_id);
        res->data = (uint8_t *) strdup(res_msg_buf);
    }
    else { res->data = (uint8_t *) strdup("Failed to leave channel"); }

    res_size = cpt_serialize_response(res, res_buf);
    tcp_server_send(info->current_id, res_buf, res_size);
    cpt_response_destroy(res);
}


int login_event(CptServerInfo * info)
{
    CptRequest * req;
    CptResponse * res;
    int login_res, new_fd;
    size_t res_size, req_size;
    uint8_t res_buf[MD_BUFF_SIZE] = {0};
    uint8_t req_buf[MD_BUFF_SIZE] = {0};

    if ( ((new_fd = handle_new_accept()) != SYS_CALL_FAIL) )
    {   /* If accept system call succeeds, attempt to add user */
        info->current_id = new_fd;
        req_size = tcp_server_recv(new_fd, req_buf);
        req = cpt_parse_request(req_buf, req_size);
        login_res = cpt_login_response(info, (char *) req->msg);

        if (login_res == SUCCESS)
        { /* If login succeeded, add file desc and set the events */
            poll_fds[nfds].fd = new_fd;
            poll_fds[nfds].events = POLLIN;
            nfds++;
        }

        res = cpt_response_init(login_res);
        res->code = login_res;

        res_size = cpt_simple_response(res, res_buf);
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


void logout_event(CptServerInfo * info)
{
    int lo_res;
    lo_res = cpt_logout_response(info);

    if ( lo_res == SUCCESS )
    {
        printf("  User with ID %d logged out...\n", info->current_id);
    }
}


void create_channel_event(CptServerInfo * info, char * id_list)
{
    int cc_res;
    uint16_t ncid;
    size_t res_size;
    CptResponse * res;
    uint8_t res_buf[MD_BUFF_SIZE] = {0};

    cc_res = cpt_create_channel_response(info, id_list);
    res = cpt_response_init(cc_res);

    ncid = (info->dir->length - 1);
    res->data = (uint8_t *) &ncid;
    res->code = (uint8_t) cc_res;
    res_size = cpt_serialize_response(res, res_buf);
    tcp_server_send(info->current_id, res_buf, res_size);
    cpt_response_destroy(res);
}


void get_users_event(CptServerInfo * info, int chan_id)
{
    int gu_res;
    size_t res_size;
    uint8_t res_buf[LG_BUFF_SIZE] = {0};

    info->res = cpt_response_init();
    gu_res = cpt_get_users_response(info, chan_id);

    if ( gu_res == SUCCESS )
        { printf("GET_USERS event occurred successfully...\n"); }

    res_size = cpt_serialize_response(info->res, res_buf);
    tcp_server_send(info->current_id, res_buf, res_size);
    cpt_response_destroy(info->res);
}


void server_destroy(Channels * dir)
{
    for (int ifd = 0; ifd < nfds; ifd++)
    {
        if ( (poll_fds[ifd].fd >= 0) )
        {
            close(poll_fds[ifd].fd);
        }
    }

    if (dir)
    {
        destroy_list((LinkedList *) dir);
    }
}




bool is_revent_POLLOUT(int index)
{
    return (
            poll_fds[index].revents ==
            ( ((uint16_t) poll_fds[index].revents) & POLLOUT )
    );
}


bool is_revent_POLLIN(int index)
{
    return (
        poll_fds[index].revents ==
        ( ((uint16_t) poll_fds[index].revents) & POLLIN )
    );
}


int handle_new_accept()
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
    if ((GCFD = tcp_server_init(NULL, NULL)) != SYS_CALL_FAIL)
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
