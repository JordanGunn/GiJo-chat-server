//
// Created by jordan on 2022-03-08.
//

#include "cpt_chat_server.h"

static int GCFD;
static int nfds;
static struct pollfd poll_fds[MAX_SERVER_FDS];

bool is_revent_POLLIN(int index);

bool is_revent_POLLOUT(int index);

void create_channel_event(Channel * gc, Channels * dir, CptPacket * req, int id);

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
    int result, active_nfds, i, j;
    bool is_fatal_error, close_conn, compress_array;

    nfds = 1;
    is_fatal_error = false;
    gc = init_global_channel();
    dir = init_channel_directory(gc);
    server_info = cpt_server_info_init(gc, dir);

    // ------------------------------------------------

    /* Setup listener socket. */
    listen_socket_init( gc );

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
            if ( !(is_revent_POLLIN(i)) )
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
                    result = login_event( gc );
                } while ( (result != FAILURE) );
            }


            /* ----------------------------------------------- */
            /* handle all events from existing connected users */
            /* ----------------------------------------------- */
            else
            {
//                printf("  Event occurred on file descriptor %d\n", poll_fds[i].fd);
                close_conn = false; // !

                size_t req_size;
                CptPacket * req;
                uint8_t req_buf[LG_BUFF_SIZE] = {0};

                req = NULL;
                do { /* Receive all incoming data from socket */

                    if ( is_revent_POLLIN(i) )
                    {
                        int id;
                        id = poll_fds[i].fd;
                        req_size = tcp_server_recv(id, req_buf);
                        req = cpt_parse_packet(req_buf, req_size);

                        if (req->command == LOGOUT)
                        {
                            logout_event(gc, dir, id);
                            cpt_packet_destroy(req);
                            close_conn = true; break;
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
                            create_channel_event(gc, dir, req, id);
                            cpt_packet_destroy(req);
                        }

                        if (req->command == LEAVE_CHANNEL)
                        {
                            puts("LEAVE_CHANNEL event");
                        }

                        if (req->command == JOIN_CHANNEL)
                        {
                            puts("JOIN_CHANNEL event");
                        }

                        /* Check if something went wrong other than EWOULDBLOCK */
                        if ( req_size < 0 )
                        {
                            if ( errno != EWOULDBLOCK )
                            {
                                perror("  recv() failed...");
                                close_conn = true; // !
                            }
                            break;
                        }

                        /* Check if connection closed by client */
                        if (req_size == 0 )
                        {
                            printf("  Connection closed\n");
                            close_conn = true;
                            break;
                        }
                    } else { break; }

//                    if ( (result < 0) )
//                    {
//                        perror("  send() failed");
//                        close_conn = true;
//                        break;
//                    }
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

    server_destroy( dir );  /* Close existing sockets before ending */
}


void create_channel_event(Channel * gc, Channels * dir, CptPacket * req, int id)
{
    int cc_res;
    size_t res_size;
    CptResponse res;
    uint8_t res_buf[MD_BUFF_SIZE] = {0};
    cc_res = cpt_create_channel_response(gc, dir, req, id);

    res.code = cc_res; res.fd = id;
    res_size = cpt_simple_response(&res, res_buf);
    tcp_server_send(id, res_buf, res_size);
}


bool is_revent_POLLOUT(int index)
{
    return (
            poll_fds[index].revents ==
            (poll_fds[index].revents & POLLOUT)
        );
}


bool is_revent_POLLIN(int index)
{
    if (poll_fds[index].fd >= 0)
    {
        return (
            poll_fds[index].revents ==
            (poll_fds[index].revents & POLLIN)
        );
    } else { return false; }
}


void logout_event(Channel *gc, Channels * dir, int id)
{
    int lo_res;
    CptResponse res;
    lo_res = cpt_logout_response(gc, dir, id);
    res.code = lo_res;
    res.fd = id;
    if ( lo_res == SUCCESS )
    {
        printf("  User with ID %d logged out...\n", id);
    }
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


int login_event(Channel * gc)
{
    CptResponse res;
    CptPacket * packet;
    int login_res, new_fd;
    size_t res_size, req_size;
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
            poll_fds[nfds].fd = new_fd;
            poll_fds[nfds].events = POLLIN;
            poll_fds[CHANNEL_ZERO].revents &= 0;
            nfds++;
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
