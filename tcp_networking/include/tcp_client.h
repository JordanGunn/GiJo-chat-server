//
// Created by jordan on 2022-03-08.
//

#ifndef GIJO_TCP_CLIENT_H
#define GIJO_TCP_CLIENT_H


/**
 * @brief Get client/server address information for network data transfer.
 *
 * @param host  IP or url of target destination.
 * @param port  Port of target destination.
 * @return      Pointer to addrinfo struct or NULL if failure.
 */
struct addrinfo * tcp_client_addr(const char * host, const char * port);


/**
 * @brief Create a socket to write to server.
 *
 * @param serv_info Result returned from setup_client_addr().
 * @return          Socket file descriptor.
 */
int tcp_client_socket(struct addrinfo * serv_info);


/**
 * Set custom socket options.
 *
 * @param sock_fd   Socket file descriptor.
 * @param sock_opt  Socket option to set (see Posix Manual for details.)
 * @return          0 if success, -1 if failure.
 */
int tcp_client_sock_opt(int sock_fd, int sock_opt);


/**
 * @brief Establish TCP connection to server.
 *
 * @param sock_fd   Socket file descriptor.
 * @param serv_info Return value from tcp_init_addr().
 * @return          0 if successful, -1 if failure.
 */
int tcp_client_connect(int sock_fd, struct addrinfo * serv_info);


/**
 * @brief Send init params to the server.
 *
 * @param sock_fd   Socket file descriptor.
 * @param msg       Initialization param(s).
 */
void tcp_client_send(int sock_fd, uint8_t * data);


/**
 * @brief Drive the TCP client init functions.
 *
 * Calls all tcp client init functions to send params to server.
 *
 * @param host  IP or url of target destination.
 * @param port  Port of target destination.
 * @return      0 if successful, -1 if failure.
 */
int init_send(const char * host, const char * port, char * init);


/**
 * @brief Wait until a certain point in time.
 *
 * @param time_point start time.
 */
void countdown(long time_point);


/**
 * @brief Get a timepoint in seconds.
 *
 * @param client_time   An absolute point in time.
 * @return              Time in seconds.
 */
long time_in_sec(char * client_time);


#endif //GIJO_TCP_CLIENT_H
