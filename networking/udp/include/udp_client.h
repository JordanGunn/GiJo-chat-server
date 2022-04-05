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
struct addrinfo * setup_client_addr(const char * host, const char * port);


/**
 * @brief Create a socket to write to server.
 *
 * @param serv_info Result returned from setup_client_addr().
 * @return          Socket file descriptor.
 */
int create_client_socket(struct addrinfo * serv_info);


/**
 * @brief Send bytes to server.
 *
 * @param serv_info Result returned from setup_client_addr().
 * @param sock      Socket file descriptor.
 * @param msg       Message to be sent.
 */
void send_bytes(struct addrinfo * serv_info, int sock, char * msg);


/**
 * Wait until some moment in time.
 *
 * @param time_point The current time in seconds.
 */
void countdown(long time_point);


/**
 * Convert time in format HH:MM to Seconds.
 *
 * @param client_time   Time passed through client init params (format HH:MM)
 * @return              Client time in seconds.
 */
long time_in_sec(char * client_time);


/**
 * Generate a message of "size" bytes.
 *
 * @param id    Prepended id.
 * @param size  Number of bytes for message.
 * @return      Pointer to message.
 */
char * build_msg(int id, size_t size);

#endif // DC_SHELL_SHELL_H