#ifndef DC_SHELL_SHELL_H
#define DC_SHELL_SHELL_H

#include <netdb.h>
#include "common.h"


#define TEST_PORT_UDP "8888"


/**
 * @brief Get client/server address information for network data transfer.
 *
 * @param port  Port of target destination.
 * @return      Pointer to addrinfo struct or NULL if failure.
 */
struct addrinfo * udp_client_addr(const char * host, const char * port);


/**
 * @brief Create a socket to write to server.
 *
 * @param serv_info Result returned from setup_client_addr().
 * @return          Socket file descriptor.
 */
int udp_client_socket(struct addrinfo * serv_info);


/**
 * @brief Send bytes to server.
 *
 * @param serv_info Result returned from setup_client_addr().
 * @param sock      Socket file descriptor.
 * @param msg       Message to be sent.
 */
void udp_client_sendto(struct addrinfo * serv_info, int sock, char * msg);


/**
 *
 *
 * @param sock_fd
 * @param data
 * @param data_size
 * @return
 */
int udp_client_send(int sock_fd, uint8_t * data, size_t data_size);


/**
 *
 * @param sock_fd
 * @param buf
 * @return
 */
ssize_t udp_client_recv(int sock_fd, uint8_t * buf);


/**
 * Create a UDP connection.
 *
 * @param udp_fd
 * @param serv_info
 * @return
 */
int udp_client_connect(int udp_fd, struct addrinfo * serv_info);


/**
 *
 * @param host
 * @param port
 * @return
 */
int udp_client_init(const char * host, const char * port);


/**
 * Generate a message of "size" bytes.
 *
 * @param id    Prepended id.
 * @param size  Number of bytes for message.
 * @return      Pointer to message.
 */
char * build_msg(int id, size_t size);

#endif // DC_SHELL_SHELL_H
