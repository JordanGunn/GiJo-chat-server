//
// Created by jordan on 2022-04-07.
//

#ifndef CPT_CPT_VOICE_CLIENT_H
#define CPT_CPT_VOICE_CLIENT_H

#include "tcp_client.h"
#include "client_info.h"
#include "cpt_packet_builder.h"
#include "cpt_serialize.h"
#include "cpt_voice_definitions.h"


/**
 * Prepare a CREATE_CHANNEL request packet for the server.
 *
 * Prepares a CREATE_CHANNEL request to the server. If successful,
 * the resulting data in <serial_buf> will contain a CPT packet
 * with the necessary information to instruct the server to create
 * a new channel.
 *
 *      > <user_list> may be optionally passed as user selection
 *        parameters for the new Channel.
 *      > If <members> is not NULL, it will be assigned to the
 *        MSG field of the packet.
 *
 * @param cpt            CPT packet information and any other necessary data.
 * @param serial_buf     A buffer intended for storing the result.
 * @param user_list      Whitespace seperated user IDs as a string.
 * @return Size of the resulting serialized packet in <serial_buf>
*/
size_t cpt_create_vchannel(void * client_info, uint8_t * serial_buf, char * user_list);

#endif //CPT_CPT_VOICE_CLIENT_H
