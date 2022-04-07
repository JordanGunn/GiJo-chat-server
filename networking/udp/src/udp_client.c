//
// Created by jordan on 2022-01-30.
//

#include "udp_client.h"

struct addrinfo * setup_client_addr(const char * host, const char * port)
{
    struct addrinfo hints, * serv_info;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    if ( !host ) { hints.ai_flags = AI_PASSIVE; }

    if ( ((getaddrinfo(host, port, &hints, &serv_info)) < 0) )
    {
        const char * msg = "Failed to get client address info...";
        write(STDERR_FILENO, msg, strlen(msg));
        return NULL;
    }

    return serv_info;
}


//int create_client_socket(struct addrinfo * serv_info)
//{
//    int client_sock_fd;
//
//    client_sock_fd = socket(
//            serv_info->ai_family,
//            serv_info->ai_socktype,
//            serv_info->ai_protocol);
//
//    if ( client_sock_fd < 0 )
//    {
//        const char * msg = "Failed to create client socket...";
//        write(STDERR_FILENO, msg, strlen(msg));
//        return -1;
//    }
//
//    return client_sock_fd;
//}
//TODO testing connection oriented udp
int create_client_socket(struct addrinfo * serv_info)
{
    int client_sock_fd;

    client_sock_fd = socket(
            serv_info->ai_family,
            serv_info->ai_socktype,
            serv_info->ai_protocol);

    if ( client_sock_fd < 0 )
    {
        const char * msg = "Failed to create client socket...";
        write(STDERR_FILENO, msg, strlen(msg));
        return -1;
    }

    if (0 < (connect(client_sock_fd, (struct sockaddr *) serv_info, sizeof (struct sockaddr_in))))
    {
        puts("Connect failed..");
        return -1;
    }

    return client_sock_fd;
}


void send_bytes(struct addrinfo * serv_info, int sock, char * msg)
{
    ssize_t bytes_sent;

    bytes_sent = sendto(sock, msg, strlen(msg), 0,
                           serv_info->ai_addr, serv_info->ai_addrlen);

    if ( bytes_sent < 0 )
    {
        const char * err_msg = "Failed to send message to server...\n";
        write(STDERR_FILENO, err_msg, strlen(err_msg));
    }
}


char * build_msg(int id, size_t size)
{
    char * msg;
    char id_buff[10] = {0};
    sprintf(id_buff, "%d-", id);
    msg = malloc((unsigned long) size + 1);

    if ( !msg )
    {
        printf("Failed to allocate memory...\n");
        return NULL;
    }

    memcpy(msg, id_buff, strlen(id_buff));
    memset(msg + strlen(id_buff), 'a', (unsigned long)size - strlen(id_buff));
    msg[size] = '\0';

    return msg;
}
