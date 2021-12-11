//
// Created by jordan on 2021-12-08.
//

#include <sys/types.h>
#include <arpa/inet.h>
#include <dc_posix/sys/dc_socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

#define DEFAULT_SOCKET_TYPE 0

int main(void)
{
    uint16_t port = 8081;
    int client_socket_fd;
    int connection_status;
    char buffer[255];
    char net_ip[] = "10.65.103.239";
    char local_ip[] = "127.0.0.1";
    char tony_dc_ip[] = "192.168.1.110";

    // ---------------------------------
    // create a socket for the client
    // ---------------------------------
    //      AF_INET (socket domain)         - This is an internet socket
    //      SOCK_STREAM (transport proto.)  - default proto. TCP
    client_socket_fd  = socket(AF_INET, SOCK_STREAM, DEFAULT_SOCKET_TYPE);

    // ---------------------------------
    // specify an address for the socket
    // ---------------------------------

    // from netinet.h --> this is a param to connect()
    struct sockaddr_in server_address;

    // set properties of server_address in prep. for connect() call
    server_address.sin_family = AF_INET;  // "socket domain" --> this is an internet socket
    server_address.sin_port = htons(port); // takes int port and converts it to data structure require for server_address struct
    server_address.sin_addr.s_addr = inet_addr(tony_dc_ip); // accept any incoming connection from local machine

    // ---------------------------------
    // Connect to remote socket
    // ---------------------------------
    //
    //                              | ------------------------------------------------ |
    //                              |          POINT BELOW IS MUCHO IMPORTANTE'        |
    //                              | -------------------------------------------------|
    //                              |                                                  |
    // arg1: client socket fd   |   arg2: sock_addr pointer !!casted from &sockaddr_in!!    |  arg3: sizeof(sockaddr_in)
    connection_status = connect(
            client_socket_fd,
            (struct sockaddr *) &server_address,
            sizeof(server_address)
            );

    // ---------------------------------
    // Retrieve data from the socket
    // ---------------------------------

    // error handling before receive (connect() returns -1 if unsuccessful)
    if (connection_status < 0)
    {
        perror("Failed to connect to host");
    }
    recv(client_socket_fd, &buffer, sizeof(buffer), 0);  // last param is optional.. 0 is default

    puts("DATA RECEIVED FROM HOST:");
    puts(buffer);

    close(client_socket_fd);
}

