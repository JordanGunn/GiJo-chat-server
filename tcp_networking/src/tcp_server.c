//
// Created by jordan on 2022-02-19.
//

#include "../include/tcp_init_server.h"


struct addrinfo * tcp_server_addr(const char * ip, const char * port)
{
    struct addrinfo hints, * serv_info;

    port = (port) ? port : TEST_PORT_TCP;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (!ip) { hints.ai_flags = AI_PASSIVE; }

    if ( (getaddrinfo(ip, port, &hints, &serv_info)) == -1 )
    {
        const char * msg = "Failed to get server address info...\n";
        write(STDERR_FILENO, msg, strlen(msg));
        return NULL;
    }

    return serv_info;
}


int tcp_listen_socket(struct addrinfo * serv_info)
{
    int server_listen_fd;

    server_listen_fd = socket(serv_info->ai_family, serv_info->ai_socktype, serv_info->ai_protocol);

    if (error_has_error(err))
    {
        const char * msg = "Failed to create client socket...\n";
        write(STDERR_FILENO, msg, strlen(msg));
        return -1;
    }

    return server_listen_fd;
}


int tcp_server_sock_opt(int sock_fd, int sock_opt)
{
    int result, opt_len;

    opt_len = 1;
    result = setsockopt(sock_fd, SOL_SOCKET, sock_opt, &opt_len, sizeof(opt_len));

    if (error_has_error(err))
    {
        const char * msg = "Failed to set tcp socket option...\n";
        write(STDERR_FILENO, msg, strlen(msg));
    }

    return result;
}


int tcp_server_bind(struct addrinfo * serv_info, int listen_fd)
{
    int result;
    result = bind(listen_fd, serv_info->ai_addr, serv_info->ai_addrlen);

    if (error_has_error(err))
    {
        const char * msg = "Failed to create client socket...\n";
        write(STDERR_FILENO, msg, strlen(msg));
    }

    freeaddrinfo(serv_info);
    return result;
}


int tcp_server_listen(int listen_fd)
{
    int result;
    result = listen(listen_fd, LISTEN_BACKLOG);

    if (error_has_error(err))
    {
        const char * msg = "Failed to create client TCP socket...\n";
        write(STDERR_FILENO, msg, strlen(msg));
    }
    else
    {
        printf("Listening for tcp client connections...\n");
    }

    return result;
}


int tcp_server_accept(struct sockaddr_storage * client_addr, int listen_fd)
{
    int fd;
    socklen_t addr_len;

    addr_len = sizeof(*client_addr);
    fd = accept(listen_fd,(struct sockaddr *) client_addr, &addr_len);

    if (error_has_error(err))
    {
        const char * msg = "Failed to accept client connection...\n";
        write(STDERR_FILENO, msg, strlen(msg));
        return -1;
    }

    return fd;
}


char * tcp_server_recv(int sock_fd)
{
    ssize_t bytes_received;
    char buff[LG_BUFF];
    char * received;
    size_t msg_len;


    bytes_received = recv(sock_fd, buff, sizeof(buff), 0);
    if (error_has_error(err))
    {
        const char * msg = "Failed to receive data from client...\n";
        write(STDERR_FILENO, msg, strlen(msg));
        return NULL;
    }

    msg_len = strlen(buff);
    received = malloc( msg_len + 1);
    if (error_has_error(err)) { return NULL; }
    memcpy(received, buff, strlen(buff));
    received[msg_len] = '\0';

    return received;
}


char *init_recv(char *ip, const char *port, int *listen_fd)
{
    char * init;
    int sock_fd;
    struct addrinfo * addr;
    struct sockaddr_storage client_addr;

    port = (port) ? port : TEST_PORT_TCP;
    ip = (ip) ? ip : LOCAL_NET_IP;

    addr = tcp_server_addr(ip, port);
    *listen_fd = tcp_listen_socket(addr);
    tcp_server_sock_opt(*listen_fd, SO_REUSEADDR);
    tcp_server_bind(addr, *listen_fd);
    tcp_server_listen(*listen_fd);
    sock_fd = tcp_server_accept(&client_addr, *listen_fd);
    if (sock_fd != -1)
    {
        init = tcp_server_recv(sock_fd);
        return init;
    }

    return NULL;
}