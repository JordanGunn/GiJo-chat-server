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
    // ============================================ //
    //       I N I T I A L    S E T U P             //
    // ============================================ //
    ServerInfo * info;
    Channel * gc; Channels * dir;

    nfds = 1;
    close_conn = false;
    is_fatal_error = false;
    gc = init_global_channel();
    dir = init_channel_directory(gc);
    info = cpt_server_info_init(gc, dir);

    listen_socket_init( gc );
    // ============================================ //

    do {
        printf("Waiting on poll...\n");
        result = poll(poll_fds, (nfds_t) nfds, POLL_TIMEOUT_5M);

        if ( should_end_event_loop(result) ) { break; }

        active_nfds = nfds; /* Number of fds with events ready */
        for (i = 0; i < active_nfds; i++)
        {
            info->current_id = poll_fds[i].fd;
            if ( poll_fds[i].revents == 0 ) { continue; }

            if ( !(is_revent_POLLIN(i)) )
            {
                printf("Unexpected event: %d\n", poll_fds[i].revents);
                is_fatal_error = true; break;
            }

            if ( poll_fds[i].fd == GCFD )
            {
                printf("  New connections found...\n");
                do
                { /* Check accept backlog for login attempts */
                    result = login_event(info);
                    if ( result == SUCCESS )
                    {
                        poll_fds[nfds].fd = info->current_id;
                        poll_fds[nfds].events = POLLIN;
                        nfds++;
                    }
                } while ( result != FAILURE ); }

            /* ----------------------------------------------- */
            /* handle all events from existing connected users */
            /* ----------------------------------------------- */
            else {
                do {
                    if ( is_revent_POLLIN(i) )
                    {
                        close_conn = false;
                        info->current_id = poll_fds[i].fd;

                        /* Receive the incoming data */
                        CptRequest * req; ssize_t req_size;
                        uint8_t req_buf[LG_BUFF_SIZE] = {0};
                        req_size = tcp_server_recv(info->current_id, req_buf);
                        req = cpt_parse_request(req_buf, (size_t) req_size);

                        /* Handle the request */
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

                } while ( true );  /* END OF REQUEST LOOP */

                if ( close_conn )
                { close_connections(); }
            }
        }

        if ( compress_array )
        { compress_fds(); }


    } while ( !is_fatal_error );
    cpt_server_info_destroy(info);
}


bool is_revent_POLLIN(int index)
{
    return (
        poll_fds[index].revents ==
        ( ((uint16_t) poll_fds[index].revents) & POLLIN )
    );
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


void close_connections()
{
    close(poll_fds[i].fd);
    poll_fds[i].fd = -1;
    compress_array = true;
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
