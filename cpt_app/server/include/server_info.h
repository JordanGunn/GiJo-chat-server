//
// Created by jordan on 2022-04-05.
//

#ifndef CPT_SERVER_INFO_H
#define CPT_SERVER_INFO_H

#include "cpt_packet_builder.h"
#include "cpt_definitions.h"
#include "cpt_types.h"
#include "channel.h"

/**
 * ServerInfo object.
 *
 * Holds all useful or relevant information for
 * server side application programming. This
 * includes sending/receiving messages, storing
 * active user channels, etc.
 */
typedef struct server_info ServerInfo;
struct server_info
{
    int current_id;
    int channel_id;

    CptResponse * res;
    Channels * dir;
    Channel * gc;
};


/**
 * Initialize and allocate all necessary server data structures.
 *
 * Initializes and allocates necessary memory for data structure
 * object pointers.
 *
 * @param gc   Pointer to the GlobalChannel object.
 * @param dir  Pointer to a LinkedList of Channel objects.
 * @return     Pointer to CptServerInfo object.
 */
ServerInfo * cpt_server_info_init(Channel * gc, Channels * dir);


/**
 * Destroy te ServerInfo object.
 *
 * Free any allocated memory and set pointer to NULL.
 *
 * @param gc   Pointer to the GlobalChannel object.
 * @return     Pointer to CptServerInfo object.
 */
ServerInfo * cpt_server_info_destroy(ServerInfo * server_info);


#endif //CPT_SERVER_INFO_H
