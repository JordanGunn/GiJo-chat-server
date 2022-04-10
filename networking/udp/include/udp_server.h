#ifndef CPT_UDP_SERVER_H
#define CPT_UDP_SERVER_H

#include <netdb.h>
#include "common.h"


/**
 *
 * @param env   DC posix environment.
 * @param port  Port to listen on.
 * @return      Pointer to server address info struct.
 */
struct addrinfo * udp_server_addr(const char * ip, const char * port);


/**
 * @brief Create a server socket for listening.
 *
 * @param env       DC posix environment.
 * @param err       DC error handling.
 * @param serv_addr Server address info.
 * @return          Socket file descriptor.
 */
int udp_server_socket(struct addrinfo * serv_addr);


/**
 * @brief Bind to server socket to port.
 *
 * @param serv_addr Server address info.
 * @param sock      Server size socket file descriptor.
 * @return          0 if successful, -1 if failed.
 */
int udp_server_bind(int sock, struct addrinfo * serv_addr);


/**
 * @brief Receive incoming bytes from client(s).
 *
 * @param env   DC posix environment.
 * @param err   DC error handling.
 * @param sock  Server size socket file descriptor.
 */
ssize_t udp_server_recv(int sock_fd, uint8_t * buf);


/**
 *
 * @param sock
 * @param stream
 * @return
 */
int udp_server_recvfrom(int sock, FILE * stream);


/**
 *
 * @param sock_fd
 * @param data
 * @param data_size
 * @return
 */
int udp_server_send(int sock_fd, uint8_t * data, size_t data_size);


/**
 *
 * @param fd
 */
void udp_get_remote_info(int fd, char * port_buf, char * ip_buf);


/**
 *
 * @param host
 * @param port
 * @return
 */
int udp_server_sock_init(const char * host, const char * port);


/**
 *
 * @param udp_fd
 * @param client_addr
 * @return
 */
int udp_server_connect(int udp_fd, struct addrinfo * client_addr);


/**
 *
 * @param host
 * @param port
 * @return
 */
int udp_server_sock_r(const char * host, const char * port);


/**
 *
 * @param host
 * @param port
 * @return
 */
int udp_server_sock_s(const char * host, const char * port);

#endif // CPT_UDP_SERVER_H
