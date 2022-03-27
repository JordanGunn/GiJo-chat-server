//
// Created by jordan on 2022-02-19.
//

#include "tcp_server.h"
#include "tcp_server_config.h"


struct addrinfo * tcp_server_addr(const char * ip, const char * port)
{
    struct addrinfo hints, * serv_info;

    port = (port) ? port : PORT_8080;
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


void check_fd_err(int fd, int res);

int tcp_listen_socket(struct addrinfo * serv_info)
{
    int server_listen_fd;

    server_listen_fd = socket(serv_info->ai_family, serv_info->ai_socktype, serv_info->ai_protocol);

    if (server_listen_fd < 0)
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

    if ( result < 0 )
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

    if ( result < 0 )
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
    result = listen(listen_fd, LISTEN_BACKLOG_32);

    if ( result < 0 )
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

    if ( fd < 0 )
    {
        const char * msg = "  Done checking accept queue...\n";
        write(STDOUT_FILENO, msg, strlen(msg));
        return -1;
    }

    return fd;
}


ssize_t tcp_server_recv(int sock_fd, uint8_t * req_buf)
{
    ssize_t req_size;

    req_size = (int) recv(sock_fd, req_buf, LG_BUFF_SIZE, 0);
    if ( req_size < 0 )
    {
        const char * msg = "Failed to receive data from client...\n";
        printf("Error: %s\n", strerror(errno));
        write(STDERR_FILENO, msg, strlen(msg));
    }

    return req_size;
}


int tcp_server_send(int sock_fd, uint8_t * data, size_t data_size)
{
    ssize_t bytes_sent;

    bytes_sent = send(sock_fd, data, data_size, 0);
    if (bytes_sent < 0)
    {
        const char * err_msg = "Failed to send bytes to server...";
        write(STDERR_FILENO, err_msg, strlen(err_msg));
        exit(EXIT_FAILURE);
    }

    return (bytes_sent) ? 0 : -1;
}


int tcp_server_init(char * host, const char *port)
{
    int fd, res, on = 1;
    struct addrinfo * addr;

    port = (port) ? port : PORT_8080;
    host = (host) ? host : IP_LOCAL_LB;

    addr = tcp_server_addr(host, port);
    if ( ((fd = tcp_listen_socket(addr)) >= 0) )
    {
        /* Make sure we can keep using the socket */
        res = tcp_server_sock_opt(fd, SO_REUSEADDR);
        check_fd_err(fd, res);

        /* Set socket to non-blocking */
        res = ioctl(fd, FIONBIO, (char *)&on);
        check_fd_err(fd, res);

        /* bind the socket to the address */
        res = tcp_server_bind(addr, fd);
        check_fd_err(fd, res);

        /* listen for incoming connections */
        res = tcp_server_listen(fd);
        check_fd_err(fd, res);
    }

    return fd;
}


void check_fd_err(int fd, int res)
{
    if (res < 0 )
    {
        printf("Error: %s\n", strerror(errno));
        close(fd); exit(EXIT_FAILURE);
    }
}
