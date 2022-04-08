//
// Created by jordan on 2022-04-08.
//

#ifndef CPT_CPT_VOICE_SERVER_H
#define CPT_CPT_VOICE_SERVER_H

#include "server_info.h"
#include "tcp_server.h"
#include "cpt_voice_definitions.h"


/**
 * Handle a received 'CREATE_CHANNEL' protocol message.
 *
 * Uses information in a received CptRequest to handle
 * a CREATE_CHANNEL protocol message from a connected client.
 *
 * If a <user_list> was received in the MSG field of the packet,
 * function will also parse the <user_list> string and attempt
 * to add the requested user IDs to the channel.
 *
 * If <id_list> is NULL, function will create a new channel with
 * only the requesting user within it.
 *
 * @param server_info   Server data structures and information.
 * @param id_list       ID list from MSG field of received CPT packet.
 * @return Status Code (SUCCESS if successful, other if failure).
 */
int cpt_create_vchannel_response(void * server_info, char * id_list);

#endif //CPT_CPT_VOICE_SERVER_H
