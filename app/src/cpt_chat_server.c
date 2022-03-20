//
// Created by jordan on 2022-03-08.
//

#include "cpt_chat_server.h"

static int GCFD;
static int nfds;
static Channel * global_channel;
static Channels channel_directory;
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
    global_channel = init_global_channel();
    channel_directory = init_channel_directory(global_channel);
    // ------------------------------------------------

    /* Setup listener socket. */
    listen_socket_init();

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
            if ( poll_fds[i].revents != (POLLIN) )
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
                } while ( (result != -1) );
            }


            /* ----------------------------------------------- */
            /* handle all events from existing connected users */
            /* ----------------------------------------------- */
            else
            {
                printf("  Event occurred on file descriptor %d\n", poll_fds[i].fd);
                close_conn = false; // !

                do { /* Receive all incoming data from socket */
                    uint8_t * data;
                    if (poll_fds[i].revents & POLLIN)
                    {
                        data = (uint8_t *)tcp_server_recv(poll_fds[i].fd, &result);
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

                    /* Echo data back to client */
                    printf("  %d bytes received\n", result);
                    result = tcp_server_send(poll_fds[i].fd, data, 20);
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

void server_destroy() {
    for (int ifd = 0; ifd < nfds; ifd++)
    {
        if ( (poll_fds[ifd].fd >= 0) )
        {
            close(poll_fds[ifd].fd);
        }
    }

    if (channel_directory)
    {
        destroy_list((LinkedList *) channel_directory);
    }
    global_channel = NULL;
}


int login_event()
{
    uint8_t * data;
    CptPacket * packet;
    int login_res, new_fd;
    uint8_t serial_size;
    uint8_t serial_buf[MD_BUFF_SIZE];
    struct sockaddr_storage client_addr;

    new_fd = tcp_server_accept(&client_addr, poll_fds[CHANNEL_ZERO].fd);
    if ( new_fd < 0 ) /* Accept will fail safely with EWOULDBLOCK */
    {
        if (errno != EWOULDBLOCK) /* if errno is not EWOULDBLOCK, fatal error occurred */
        {
            perror("  accept()");
            return new_fd;
        }
    }
    data = (uint8_t *)tcp_server_recv(new_fd, &login_res);
    packet = cpt_parse_packet(data);
    login_res = cpt_handle_login(global_channel, packet, new_fd);
    if (login_res == SUCCESS)
    {
        poll_fds[nfds  ].fd = new_fd;
        poll_fds[nfds++].events = POLLIN | POLLOUT;
        serial_size = cpt_serialize_packet(packet, serial_buf);
        tcp_server_send(new_fd, serial_buf, serial_size);
    }
    return login_res;
}


void listen_socket_init()
{
    memset(poll_fds, 0 , sizeof(poll_fds));
    if ((GCFD = tcp_server_init(NULL, NULL)) > 0)
    {
        global_channel->fd = GCFD;
        poll_fds[CHANNEL_ZERO].fd = global_channel->fd;
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
