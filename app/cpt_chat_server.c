//
// Created by jordan on 2022-03-08.
//

#include "cpt_chat_server.h"

static int gc_fd;
static int nfds;
static Channel * global_channel;
static Channels channel_directory;
static struct pollfd poll_fds[MAX_SERVER_FDS];

int main(void)
{
    return 0;
}


void run()
{

    // ===============================================
    //    I N I T I A L    S E T U P
    // ===============================================
    int poll_result;
    bool is_fatal_error;
    char * char_pointer;

    nfds = 1;
    is_fatal_error = false;
    global_channel = init_global_channel();
    channel_directory = init_channel_directory(global_channel);
    // ------------------------------------------------



    // ==================================================
    /* Setup listener socket. */
    // ==================================================
    if ((gc_fd = tcp_server_init(NULL, NULL)) > 0)
    {
        global_channel->fd = gc_fd;
        poll_fds[CHANNEL_ZERO].fd = global_channel->fd;
        poll_fds[CHANNEL_ZERO].events = POLLIN;
    }
    else { exit( SERV_FATAL ); }
    // -------------------------------------------------



    // **************************************************
    /*              E V E N T    L O O P               */
    // **************************************************
    while ( !is_fatal_error )
    {
        poll_result = poll(poll_fds, nfds, POLL_TIMEOUT_0);
        if ( poll_result < 0 ) { is_fatal_error = true; }
    // **************************************************



        // ==================================================
        /* L O G I N   E V E N T    H A N D L E R          */
        // ==================================================
        if ( poll_fds[CHANNEL_ZERO].revents & POLLIN )
        {
            CptPacket * packet;
            char_pointer = tcp_server_recv(poll_fds[CHANNEL_ZERO].fd);
            packet = cpt_parse_packet((uint8_t *)char_pointer);

            if (packet->command == (uint8_t)LOGIN)
            {
                int login_res;
                struct sockaddr_storage client_addr;
                int new_fd = tcp_server_accept(
                        &client_addr, poll_fds[CHANNEL_ZERO].fd
                    );
                login_res = cpt_handle_login(global_channel, packet, new_fd);
                if (login_res == SUCCESS)
                {
                    poll_fds[nfds++].fd = new_fd;
                    poll_fds[nfds].events = POLLIN | POLLOUT;
                }
            }
        }
        // -------------------------------------------------------
    }
}




