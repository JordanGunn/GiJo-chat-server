//
// Created by jordan on 2022-04-05.
//

#ifndef CPT_CLIENT_INFO_H
#define CPT_CLIENT_INFO_H

#include "common.h"
#include "cpt_types.h"


/**
 * Client info object.
 *
 * Holds all useful or relevant information for
 * client side application programming. This
 * includes sending/receiving messages, storing
 * active user channels, etc.
 */
typedef struct client_info ClientInfo;
struct client_info
{
    int fd;
    int code;
    char * ip;
    char * port;
    char * name;
    uint16_t channel;
    CptRequest * packet;
    LinkedList * channels;
};


/**
 * Create a CptClientInfo object.
 *
 * Contains necessary information to
 * to create a TCP connection with the
 * server and send data back and forth.
 *
 * Initializes all necessary fields and
 * allocates any memory necessary for the
 * object.
 *
 * @param port  Server port number.
 * @param ip    Server IP or URL.
 * @return CptClientInfo object.
 */
ClientInfo * cpt_init_client_info(char * port, char * ip);


/**
 * Destroy CptRequestInfo object.
 *
 * Frees any memory necessary for the
 * object and set all pointers to NULL.
 *
 * @param packet_info CptRequestInfo object.
 */
void cpt_destroy_client_info(ClientInfo * client_info);

#endif //CPT_CLIENT_INFO_H
