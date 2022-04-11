//
// Created by jordan on 2022-01-27.
//

#include "udp_server.h"
#include "common.h"

struct addrinfo * udp_server_addr(const char * ip, const char * port)
{
    struct addrinfo hints, * serv_info;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    if ( !ip )  { hints.ai_flags = AI_PASSIVE; }

    if ( (getaddrinfo(ip, port, &hints, &serv_info)) == -1 )
    {
        const char * msg = "Failed to get server address info...\n";
        write(STDERR_FILENO, msg, strlen(msg));
        return NULL;
    }

    return serv_info;
}




int udp_server_socket(struct addrinfo * serv_addr)
{
    int server_socket_fd;

    server_socket_fd = socket(serv_addr->ai_family, serv_addr->ai_socktype, serv_addr->ai_protocol);

    if ( server_socket_fd < 0 )
    {
        const char * msg = "Failed to create server socket...\n";
        write(STDERR_FILENO, msg, strlen(msg));
        return -1;
    }

    return server_socket_fd;
}


int udp_server_bind(int sock, struct addrinfo * serv_addr)
{
    int result;

    result = bind(sock, serv_addr->ai_addr, serv_addr->ai_addrlen);
    if ( result < 0 )
    {
        const char * msg = "Failed to bind socket...\n";
        write(STDERR_FILENO, msg, strlen(msg));
        return -1;
    }

    freeaddrinfo(serv_addr);
    return result;
}


ssize_t udp_server_recv(int sock_fd, uint8_t * buf)
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


void udp_get_remote_info(int fd, char * port_buf, char * ip_buf)
{
    int port;
    socklen_t len;
    struct sockaddr_storage addr;
    char ipstr[INET6_ADDRSTRLEN] = {0};

    len = sizeof addr;
    getpeername(fd, (struct sockaddr*)&addr, &len);

    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
    port = ntohs(s->sin_port);
    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);

    sprintf(port_buf, "%d", port);
    memcpy(ip_buf, ipstr, strlen(ipstr) + 1);

    printf("Peer IP address: %s\n", ipstr);
    printf("Peer port : %d\n", port);
}


int udp_server_recvfrom(int sock, FILE * stream)
{
    ssize_t bytes_recvd;
    socklen_t client_addr_len;
    char buffer[SM_BUFF_SIZE] = {0};

    struct sockaddr_storage client_addr;

    client_addr_len = sizeof(client_addr);
    bytes_recvd = recvfrom(sock, buffer, SM_BUFF_SIZE + 1, 0,
                           (struct sockaddr *) &client_addr, &client_addr_len);


    if ( bytes_recvd < 0 )
    {
        if ( ((errno != EWOULDBLOCK) && (errno != EAGAIN)) )
        {
            const char * err_msg = "Failed to receive bytes from client...\n";
            write(STDERR_FILENO, err_msg, strlen(err_msg));
        }
    }

    return ( (int) bytes_recvd );
}


int udp_server_sock_init(const char * host, const char * port)
{
    int udp_fd;
    char * ip_copy, * port_copy;
    struct addrinfo * server_addr;

    ip_copy = strdup(host);
    port_copy = strdup(port);

    server_addr = udp_server_addr(ip_copy, port_copy);

    if ( server_addr )
    {
        udp_fd = udp_server_socket(server_addr);
    }


    free(ip_copy); ip_copy = NULL;
    free(port_copy); port_copy = NULL;
    return ( server_addr ) ? udp_fd : SYS_CALL_FAIL;
}


int udp_server_sock_s(const char * host, const char * port)
{
    int udp_fd, conn_res;
    char * ip_copy, * port_copy;
    struct addrinfo * server_addr;

    ip_copy = strdup(host);
    port_copy = strdup(port);

    server_addr = udp_server_addr(ip_copy, port_copy);

    conn_res = -1;
    if ( server_addr )
    {
        if ( ((udp_fd = udp_server_socket(server_addr)) != SYS_CALL_FAIL) )
        {
            conn_res = udp_server_connect(udp_fd, server_addr);
        }
    }

    free(ip_copy); ip_copy = NULL;
    free(port_copy); port_copy = NULL;
    return (conn_res == SYS_CALL_FAIL ) ? conn_res : udp_fd;
}


int udp_server_sock_r(const char * host, const char * port)
{
    int udp_fd, bind_res;
    char * ip_copy, * port_copy;
    struct addrinfo * server_addr;

    ip_copy = strdup(host);
    port_copy = strdup(port);

    server_addr = udp_server_addr(ip_copy, port_copy);

    bind_res = -1;
    if ( server_addr )
    {
        if ( ((udp_fd = udp_server_socket(server_addr)) != SYS_CALL_FAIL) )
        {
            bind_res = udp_server_bind(udp_fd, server_addr);
        }
    }

    free(ip_copy); ip_copy = NULL;
    free(port_copy); port_copy = NULL;
    return (bind_res == SYS_CALL_FAIL ) ? bind_res : udp_fd;
}


int udp_server_send(int sock_fd, uint8_t * data, size_t data_size)
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


int udp_server_connect(int udp_fd, struct addrinfo * client_addr)
{
    int result;

    result = connect(udp_fd, client_addr->ai_addr, client_addr->ai_addrlen);

    if ( result < 0)
    {
        if (errno != EWOULDBLOCK)
        {
            const char * msg = "Failed to connect to server...\n";
            write(STDERR_FILENO, msg, strlen(msg));
            return -1;
        }
    }

    freeaddrinfo(client_addr);
    return result;
}
