//
// Created by jordan on 2022-03-08.
//

#include "tcp_client.h"


struct addrinfo * tcp_client_addr(const char * host, const char * port)
{
    struct addrinfo hints, * serv_info;

    port = (port) ? port : PORT_8888;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if ( !host ) { hints.ai_flags = AI_PASSIVE; }

    if ( (getaddrinfo(host, port, &hints, &serv_info)) < 0 )
    {
        const char * msg = "Failed to get client address info...\n";
        write(STDERR_FILENO, msg, strlen(msg));
        return NULL;
    }

    return serv_info;
}


int tcp_client_socket(struct addrinfo * serv_info)
{
    int client_sock_fd;

    client_sock_fd = socket(serv_info->ai_family, serv_info->ai_socktype, serv_info->ai_protocol);

    if (client_sock_fd < 0)
    {
        const char * msg = "Failed to create client socket...\n";
        write(STDERR_FILENO, msg, strlen(msg));
        return -1;
    }

    return client_sock_fd;
}


int tcp_client_sock_opt(int sock_fd, int sock_opt)
{
    int result, opt_len;

    opt_len = 1;
    result = setsockopt(sock_fd, SOL_SOCKET, sock_opt, &opt_len, sizeof(opt_len));

    if (result < 0)
    {
        const char * msg = "Failed to set tcp socket option...\n";
        write(STDERR_FILENO, msg, strlen(msg));
    }

    return result;
}


int tcp_client_connect(int sock_fd, struct addrinfo * serv_info)
{
    int result;

    result = connect(sock_fd, serv_info->ai_addr, serv_info->ai_addrlen);

    if ( result < 0)
    {
        const char * msg = "Failed to connect to server...\n";
        write(STDERR_FILENO, msg, strlen(msg));
        return -1;
    }

    freeaddrinfo(serv_info);
    return result;
}


int tcp_client_send(int sock_fd, uint8_t * data, size_t data_size)
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


int tcp_init_client(const char * host, const char * port)
{
    int sock_fd;
    struct addrinfo * addr;

    addr = tcp_client_addr(host, port);
    sock_fd = tcp_client_socket(addr);
    if ( (tcp_client_sock_opt(sock_fd, SO_REUSEADDR)) == -1 ) { sock_fd = -1; }
    tcp_client_connect(sock_fd, addr);

    return sock_fd;
}


ssize_t tcp_client_recv(int sock_fd, uint8_t * buff)
{
    ssize_t bytes_received;

    bytes_received = recv(sock_fd, buff, MD_BUFF_SIZE, 0);
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
