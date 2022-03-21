//
// Created by jordan on 2022-03-08.
//

#ifndef CPT_TCP_CLIENT_H
#define CPT_TCP_CLIENT_H

#include "common.h"
#include "tcp_client_config.h"

/**
 * @brief Get client/server address information for network data transfer.
 *
 * A wrapper for posix setsockopt() system call
 * that includes some minor error handling.
 *
 * @param host  IP or url of target destination.
 * @param port  Port of target destination.
 * @return      Pointer to addrinfo struct or NULL if failure.
 */
struct addrinfo * tcp_client_addr(const char * host, const char * port);


/**
 * @brief Create a socket to write to server.
 *
 * A wrapper for posix socket() system call
 * that includes some minor error handling.
 *
 * @param serv_info Result returned from setup_client_addr().
 * @return          Socket file descriptor.
 */
int tcp_client_socket(struct addrinfo * serv_info);


/**
 * @brief Set custom socket options.
 *
 * A wrapper for posix setsockopt() system call
 * that includes some minor error handling.
 *
 * @param sock_fd   Socket file descriptor.
 * @param sock_opt  Socket option to set (see Posix Manual for details.)
 * @return          0 if success, -1 if failure.
 */
int tcp_client_sock_opt(int sock_fd, int sock_opt);


/**
 * @brief Establish TCP connection to server.
 *
 * A wrapper for posix connect() system call
 * that includes some minor error handling.
 *
 * @param sock_fd   Socket file descriptor.
 * @param serv_info Return value from tcp_init_addr().
 * @return          0 if successful, -1 if failure.
 */
int tcp_client_connect(int sock_fd, struct addrinfo * serv_info);


/**
 * @brief Send a message over TCP connection.
 *
 * @param sock_fd   Socket file descriptor.
 * @param msg       Data to send.
 */
int tcp_client_send(int sock_fd, uint8_t * data, size_t data_size);


/**
 * @brief Initialize a TCP connection.
 *
 * Makes all necessary TCP posix system calls and
 * returns a file descriptor for writing to the
 * connected socket.
 *
 * @param host  IP or URL of target destination.
 * @param port  Port of target destination.
 * @return      0 if successful, -1 if failure.
 */
int tcp_init_client(const char * host, const char * port);


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


/**
 * @brief Receive data from tcp client.
 *
 * @param sock_fd   Socket file descriptor.
 * @return          Pointer to received data.
 */
size_t tcp_client_recv(int sock_fd, uint8_t * buff);


#endif //CPT_TCP_CLIENT_H
