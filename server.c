#include <sys/types.h>
#include <arpa/inet.h>
#include <dc_posix/sys/dc_socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

// file descriptor: index in the file descriptor table
//    inside os there is something called process control block
//    inside the PCB there is a structure or array called the file descriptor table
//    file table holds pointers to all resources used by the process
//        - files
//        - terminal i/o
//        - pipes
//        - sockets
//
//    When every process opens, it is given access to 3 resources:
//        0 stdin
//        1 stdout
//        2 stderr
//
//   If you create a new resource (e.g. opening a new fle)
//


#define TCP 0

void sig_handler(int num) {

}

int main() {

    int serv_fd;
    int client_fd;
    int bind_result;
    uint16_t port = 8080;

    char local_ip[] = "127.0.0.1";
    char net_ip[] ="192.168.1.106";
    char msg[255] = "SERVER RESPONSE: Hello, there!";

    struct sockaddr_in serv_addr;

    // Create the server socket for receiving connections
    serv_fd = socket(AF_INET, SOCK_STREAM, TCP);

    // Set up the address for the server
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(net_ip);

    // bind the address info to the socket, I.e. "We want to listen to this IP on this port"
    bind_result = bind(serv_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    if (bind_result < 0) {
        perror("Failed to bind to socket...");
    }

    listen(serv_fd, 1);



    // Listen for incoming connections at specific port & ip
    // Will return client fd, which creates two-way connection.
    client_fd = accept(serv_fd, NULL, NULL);

    // send some data back to the client
    send(client_fd, msg, sizeof(msg), 0);

    close(serv_fd);





    return 0;
}
