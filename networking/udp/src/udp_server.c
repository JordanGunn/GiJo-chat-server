//
// Created by jordan on 2022-01-27.
//

#include "udp_server.h"
#include "common.h"

struct addrinfo * setup_server_addr(const char * ip, const char * port)
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




int create_server_socket(struct addrinfo * serv_info)
{
    int server_socket_fd;

    server_socket_fd = socket(serv_info->ai_family, serv_info->ai_socktype, serv_info->ai_protocol);

    if ( server_socket_fd < 0 )
    {
        const char * msg = "Failed to create server socket...\n";
        write(STDERR_FILENO, msg, strlen(msg));
        return -1;
    }

    return server_socket_fd;
}


int bind_server_socket(struct addrinfo * serv_info, int sock)
{
    int result;

    result = bind(sock, serv_info->ai_addr, serv_info->ai_addrlen);
    if ( result < 0 )
    {
        const char * msg = "Failed to bind socket...\n";
        write(STDERR_FILENO, msg, strlen(msg));
        return -1;
    }

    freeaddrinfo(serv_info);
    return result;
}


int recv_bytes(int sock, FILE * stream)
{
    ssize_t bytes_recvd;
    in_port_t client_port;
    socklen_t client_addr_len;
    char buffer[SM_BUFF_SIZE] = {0};
    char ip_buffer[SM_BUFF_SIZE] = {0};

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
    else
    {
        inet_ntop(client_addr.ss_family,
                  &((struct sockaddr_in *)&client_addr)->sin_addr,
                  ip_buffer, client_addr_len);
        client_port =  ntohs(((struct sockaddr_in *)&client_addr)->sin_port);
        write_result(client_port, bytes_recvd, buffer, ip_buffer, stream);
    }

    return ( (int) bytes_recvd );
}


void write_result(in_port_t client_port, ssize_t bytes_recvd, const char *buffer, const char *ip_buffer, FILE *stream) {

    long id;
    char * rest_of_string;

    id = strtol(buffer, &rest_of_string, 10);
    fprintf(stream, "%s, %hu, %ld, %zd\r\n",
            strdup(ip_buffer), client_port, id, bytes_recvd);
}
