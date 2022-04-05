#ifndef DC_SHELL_INPUT_H
#define DC_SHELL_INPUT_H

/*
 * This file is part of dc_shell.
 *
 *  dc_shell is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Foobar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with dc_shell.  If not, see <https://www.gnu.org/licenses/>.
 */


#include <netdb.h>
#include "common.h"

#define TEST_PORT_UDP "8888"
#define TEST_PORT_TCP "8080"


/**
 *
 * @param env   DC posix environment.
 * @param port  Port to listen on.
 * @return      Pointer to server address info struct.
 */
struct addrinfo * setup_server_addr(const char * ip, const char * port);


/**
 * @brief Create a server socket for listening.
 *
 * @param env       DC posix environment.
 * @param err       DC error handling.
 * @param serv_info Server address info.
 * @return          Socket file descriptor.
 */
int create_server_socket(struct addrinfo * serv_info);


/**
 * @brief Bind to server socket to port.
 *
 * @param env       DC posix environment.
 * @param err       DC error handling.
 * @param serv_info Server address info.
 * @param sock      Server size socket file descriptor.
 * @return          0 if successful, -1 if failed.
 */
int bind_server_socket(struct addrinfo * serv_info, int sock);


/**
 * @brief Receive incoming bytes from client(s).
 *
 * @param env   DC posix environment.
 * @param err   DC error handling.
 * @param sock  Server size socket file descriptor.
 */
int recv_bytes(int sock, FILE * stream);


/**
 * Write UDP message results to stream.
 *
 * @param client_port   The incoming port number.
 * @param bytes_recvd   Number of bytes received.
 * @param buffer        Received UDP message.
 * @param ip_buffer     Received IP address.
 * @param stream        Stream to write to.
 */
void write_result(in_port_t client_port, ssize_t bytes_recvd, const char *buffer, const char *ip_buffer, FILE *stream);

#endif // DC_SHELL_INPUT_H
