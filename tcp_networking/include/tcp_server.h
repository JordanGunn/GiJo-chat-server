//
// Created by jordan on 2022-03-08.
//

#ifndef CPT_TCP_SERVER_H
#define CPT_TCP_SERVER_H

#include "common.h"
#include "tcp_server_config.h"


/**
 * @brief Get client/server address information for network data transfer.\n\n
 *
 * @param host  IP or url of target destination.
 * @param port  Port of target destination.
 * @return      Pointer to addrinfo struct or NULL if failure.
 */
struct addrinfo * tcp_server_addr(const char * ip, const char * port);


/**
 * @brief Create a socket to write to server.\n\n
 *
 * @param serv_info Result returned from setup_client_addr().
 * @return          Socket file descriptor.
 */
int tcp_listen_socket(struct addrinfo * serv_info);


/**
 * Set custom socket options.\n\n
 *
 * @param sock_fd   Socket file descriptor.
 * @param serv_info Return value from tcp_server_addr().
 * @param sock_opt  Socket option to set (see Posix Manual for details.)
 * @return          0 if success, -1 if failure.
 */
int tcp_server_sock_opt(int sock_fd, int sock_opt);


/**
 * @brief Bind server address to listener socket.\n\n
 *
 * @param listen_fd Listener socket file descriptor.
 * @param serv_info Result returned from setup_client_addr().
 * @return          Socket file descriptor.
 */
int tcp_server_bind(struct addrinfo * serv_info, int listen_fd);


/**
 * @brief Bind server address to listener socket.\n\n
 *
 * @param listen_fd Listener socket file descriptor.
 * @param serv_info Result returned from setup_client_addr().
 * @return          Socket file descriptor.
 */
int tcp_server_listen(int listen_fd);


/**
 * @brief Accept incoming tcp connection.\n\n
 *
 * @param listen_fd Listener socket file descriptor.
 * @param serv_info Result returned from setup_client_addr().
 * @return          Socket file descriptor.
 */
int tcp_server_accept(struct sockaddr_storage * client_addr, int listen_fd);


/**
 * @brief Receive data from tcp client.\n\n
 *
 * @param sock_fd   Socket file descriptor.
 * @return          Pointer to received data.
 */
char * tcp_server_recv(int sock_fd, int * result);


/**
 * @brief Drive the TCP init functions.\n\n
 *
 * Calls all tcp server init functions and returns file
 * descriptor for te listener socket.
 *
 * @param host  IP or url of target destination.
 * @param port  Port of target destination.
 * @return      Pointer to received.
 */
int tcp_server_init(char * host, const char *port);


/**
 * @brief Send data to connected client.\n\n
 *
 * Sends data a TCP connected client.
 *
 * @param sock_fd   Client socket file descriptor.
 * @param data      Data being sent.
 * @return          0 if success, -1 if failure.
 */
int tcp_server_send(int sock_fd, uint8_t * data);

#endif //CPT_TCP_SERVER_H
