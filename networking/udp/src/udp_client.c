//
// Created by jordan on 2022-01-30.
//

#include "udp_client.h"

struct addrinfo * udp_client_addr(const char * host, const char * port)
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


int udp_client_socket(struct addrinfo * serv_info)
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

    return client_sock_fd;
}


int udp_client_connect(int udp_fd, struct addrinfo * serv_info)
{
    int result;

    result = connect(udp_fd, serv_info->ai_addr, serv_info->ai_addrlen);

    if ( result < 0)
    {
        if (errno != EWOULDBLOCK)
        {
            const char * msg = "Failed to connect to server...\n";
            write(STDERR_FILENO, msg, strlen(msg));
            return -1;
        }
    }

    freeaddrinfo(serv_info);
    return result;
}


int udp_init(const char * host, const char * port)
{
    int udp_fd, conn_res;
    struct addrinfo * client_addr;

    client_addr = udp_client_addr(host, port);

    conn_res = -1;
    if ( client_addr )
    {
        if ( ((udp_fd = udp_client_socket(client_addr)) != SYS_CALL_FAIL) )
        {
            conn_res = udp_client_connect(udp_fd, client_addr);
        }
    }

    return conn_res;
}


int udp_client_send(int sock_fd, uint8_t * data, size_t data_size)
{
    ssize_t bytes_sent, bytes_left;

    bytes_left = (ssize_t) data_size;
    while ( bytes_left > 0 )
    {
        bytes_sent = send(sock_fd, data, data_size, 0);
        if ( bytes_sent < 0 )
        { break; }
        else
        {  bytes_left -= bytes_sent; }
    }

    if (bytes_sent < 0)
    {
        const char * err_msg = "Failed to send bytes to server...\n";
        write(STDERR_FILENO, err_msg, strlen(err_msg));
    }

    return (int) bytes_sent;
}


ssize_t udp_client_recv(int sock_fd, uint8_t * buf)
{
    ssize_t bytes_received;

    bytes_received = recv(sock_fd, buf, MD_BUFF_SIZE, 0);
    if ( bytes_received < 0 )
    {
        if (errno != EWOULDBLOCK || errno != EAGAIN)
        {
            const char * msg = "Failed to receive data from server...\n";
            printf("  ERROR: %s\n", strerror(errno));
            write(STDERR_FILENO, msg, strlen(msg));
        }
    }

    return bytes_received;
}



void udp_client_sendto(struct addrinfo * serv_info, int sock, char * msg)
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
