//
// Created by jordan on 2022-03-08.
//

#ifndef GIJO_TCP_SERVER_H
#define GIJO_TCP_SERVER_H

//
// Created by jordan on 2022-02-19.
//

#ifndef UDP_TESTER_TCP_INIT_SERVER_H
#define UDP_TESTER_TCP_INIT_SERVER_H

#include "common.h"


/**
 * @brief Get client/server address information for network data transfer.
 *
 * @param env   DC posix environment.
 * @param host  IP or url of target destination.
 * @param port  Port of target destination.
 * @return      Pointer to addrinfo struct or NULL if failure.
 */
struct addrinfo * tcp_server_addr(struct dc_posix_env * env, const char * ip, const char * port);


/**
 * @brief Create a socket to write to server.
 *
 * @param env       DC posix environment.
 * @param err       DC error handling.
 * @param serv_info Result returned from setup_client_addr().
 * @return          Socket file descriptor.
 */
int tcp_listen_socket(struct dc_posix_env * env, struct dc_error * err, struct addrinfo * serv_info);


/**
 * Set custom socket options.
 *
 * @param env       DC posix environment.
 * @param err       DC error handling.
 * @param sock_fd   Socket file descriptor.
 * @param serv_info Return value from tcp_server_addr().
 * @param sock_opt  Socket option to set (see Posix Manual for details.)
 * @return          0 if success, -1 if failure.
 */
int tcp_server_sock_opt(struct dc_posix_env * env, struct dc_error * err, int sock_fd, int sock_opt);


/**
 * @brief Bind server address to listener socket.
 *
 * @param env       DC posix environment.
 * @param err       DC error handling.
 * @param listen_fd Listener socket file descriptor.
 * @param serv_info Result returned from setup_client_addr().
 * @return          Socket file descriptor.
 */
int tcp_server_bind(struct dc_posix_env * env, struct dc_error * err, struct addrinfo * serv_info, int listen_fd);


/**
 * @brief Bind server address to listener socket.
 *
 * @param env       DC posix environment.
 * @param err       DC error handling.
 * @param listen_fd Listener socket file descriptor.
 * @param serv_info Result returned from setup_client_addr().
 * @return          Socket file descriptor.
 */
int tcp_server_listen(struct dc_posix_env * env, struct dc_error * err, int listen_fd);


/**
 * @brief Accept incoming tcp connection.
 *
 * @param env       DC posix environment.
 * @param err       DC error handling.
 * @param listen_fd Listener socket file descriptor.
 * @param serv_info Result returned from setup_client_addr().
 * @return          Socket file descriptor.
 */
int tcp_server_accept(struct dc_posix_env * env, struct dc_error * err, struct sockaddr_storage * client_addr, int listen_fd);



/**
 * @brief Receive data from tcp client.
 *
 * @param env       DC posix environment.
 * @param err       DC error handling.
 * @param sock_fd   Socket file descriptor.
 * @return          Pointer to received data.
 */
char * tcp_server_recv(struct dc_posix_env * env, struct dc_error * err, int sock_fd);


/**
 * @brief Drive the TCP init functions.
 *
 * Calls all tcp server init functions and returns the init params
 * received from the client.
 *
 * @param env   DC posix environment.
 * @param err   DC error handling.
 * @param host  IP or url of target destination.
 * @param port  Port of target destination.
 * @return      Pointer to received.
 */
char *init_recv(struct dc_posix_env *env, struct dc_error *err, char *ip, const char *port, int *listen_fd);

#endif //UDP_TESTER_TCP_INIT_SERVER_H

#endif //GIJO_TCP_SERVER_H
