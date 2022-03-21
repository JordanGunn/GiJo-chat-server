//
// Created by jordan on 2022-03-08.
//

#include "cpt_chat_server.h"

static int GCFD;
static int nfds;
static Channel * gc;
static Channels channel_dir;
static struct pollfd poll_fds[MAX_SERVER_FDS];

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
    int result, active_nfds, i, j;
    bool is_fatal_error, close_conn, compress_array;

    nfds = 1;
    is_fatal_error = false;
    gc = init_global_channel();
    channel_dir = init_channel_directory(gc);
    // ------------------------------------------------

    /* Setup listener socket. */
    listen_socket_init();

//    printf("POLLERR: %d\n",  POLLERR);
//    printf("POLLIN: %d\n",   POLLIN);
//    printf("POLLOUT: %d\n",  POLLOUT);
//    printf("POLLHUP: %d\n",  POLLHUP);
//    printf("POLLNVAL: %d\n", POLLNVAL);

    // **************************************************
    // ************ E V E N T ** L O O P ****************   AMERICA !!!!!!!!!!!!!!!
    // **************************************************
    do {
        printf("Waiting on poll...\n");
        result = poll(poll_fds, nfds, POLL_TIMEOUT_1M);

        if ( should_end_event_loop(result) ) { break; }

        active_nfds = nfds; /* Number of fds with events ready */
        for (i = 0; i < active_nfds; i++)
        {

            /* -------------------------------------------------- */
            /* No event on current socket, go back to top of loop */
            /* -------------------------------------------------- */
            if ( poll_fds[i].revents == 0 ) { continue; }


            /* ------------------------------------ */
            /* Unexpected event causing fatal error */
            /* ------------------------------------ */
            if (
                poll_fds[i].revents != (POLLIN) &&
                poll_fds[i].revents != (POLLOUT)
            )
            {
                printf("Unexpected event: %d\n", poll_fds[i].revents);
                is_fatal_error = true; break;
            }


            /* ------------------------------------------------ */
            /* Event on listener socket, new connection arrived */
            /* ------------------------------------------------ */
            if (poll_fds[i].fd == GCFD )
            {
                printf("New connections found...\n");
                printf("Checking queued login attempts...\n");
                do { /* Check accept backlog queue for all incoming login attempts */
                    result = login_event();
                } while ( (result != FAILURE) );
            }


            /* ----------------------------------------------- */
            /* handle all events from existing connected users */
            /* ----------------------------------------------- */
            else
            {
                printf("  Event occurred on file descriptor %d\n", poll_fds[i].fd);
                close_conn = false; // !

                do { /* Receive all incoming data from socket */
                    if (
                            poll_fds[i].revents & POLLIN ||
                            poll_fds[i].revents & POLLOUT
                    )
                    {
                        int res;
                        size_t req_size;
                        CptPacket * req;
                        uint8_t req_buf[LG_BUFF_SIZE] = {0};

                        req_size = tcp_server_recv(poll_fds[i].fd, req_buf);
                        req = cpt_parse_packet(req_buf, req_size);

                        if (req->command == LOGOUT)
                        {
                            logout_event(req, poll_fds[i].fd);
                            poll_fds[i].fd = -1; nfds--;
                        }

                        if (req->command == SEND)
                        {
                            puts("SEND event");
                        }

                        if (req->command == GET_USERS)
                        {
                            puts("GET_USERS event");
                        }

                        if (req->command == CREATE_CHANNEL)
                        {
                            puts("CREATE_CHANNEL event");
                        }

                        if (req->command == LEAVE_CHANNEL)
                        {
                            puts("LEAVE_CHANNEL event");
                        }

                        if (req->command == JOIN_CHANNEL)
                        {
                            puts("JOIN_CHANNEL event");
                        }

                        if ( result < 0 )
                        {
                            if ( errno != EWOULDBLOCK )
                            {
                                perror("  recv() failed...");
                                close_conn = true; // !
                            }
                            break;
                        }
                    }

                    /* Check if connection closed by client */
                    if (result == 0 )
                    {
                        printf("  Connection closed\n");
                        close_conn = true;
                        break;
                    }

                    if ( (result < 0) )
                    {
                        perror("  send() failed");
                        close_conn = true;
                        break;
                    }
                } while ( true );

                /* handle close connection flag if set */
                if ( close_conn )
                {
                    close(poll_fds[i].fd);
                    poll_fds[i].fd = -1; // setting fd to -1 will cause poll() to ignore this fd index
                }

            }   /* CURRENT USER CONNECTION EVENTS */

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

    server_destroy();  /* Close existing sockets before ending */
}

void logout_event(CptPacket *req, int id) {
    int lo_res;
    size_t res_size;
    uint8_t res_buf[LG_BUFF_SIZE] = {0};
    CptResponse res;
    lo_res = cpt_logout_response(gc, channel_dir, id);
    res.code = lo_res;
    res.fd = id;
    if ( lo_res == SUCCESS )
    {
        res_size = cpt_simple_response(&res, res_buf);
        tcp_server_send(id, res_buf, res_size);
    }
    cpt_packet_destroy(req);
}

void server_destroy() {
    for (int ifd = 0; ifd < nfds; ifd++)
    {
        if ( (poll_fds[ifd].fd >= 0) )
        {
            close(poll_fds[ifd].fd);
        }
    }

    if (channel_dir)
    {
        destroy_list((LinkedList *) channel_dir);
    }
    gc = NULL;
}


int login_event()
{
    size_t res_size, req_size;
    CptResponse res;
    CptPacket * packet;
    int login_res, new_fd;
    uint8_t res_buf[MD_BUFF_SIZE] = {0};
    uint8_t req_buf[MD_BUFF_SIZE] = {0};

    if ( (new_fd =  handle_new_accept()) != SYS_CALL_FAIL )
    {
        req_size = tcp_server_recv(new_fd, req_buf);
        packet = cpt_parse_packet(req_buf, req_size);
        login_res = cpt_login_response(gc, packet, new_fd);
        res.code = login_res; res.fd = new_fd;
        if (login_res == SUCCESS)
        {
            poll_fds[nfds  ].fd = new_fd;
            poll_fds[nfds++].events = POLLIN | POLLOUT;
        }
        res_size = cpt_simple_response(&res, res_buf);
        tcp_server_send(new_fd, res_buf, res_size);

        cpt_packet_destroy(packet);
    }

    return (new_fd != SYS_CALL_FAIL) ? SUCCESS : FAILURE;
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


void listen_socket_init()
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
