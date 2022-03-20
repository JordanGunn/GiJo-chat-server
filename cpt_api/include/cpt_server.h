//
// Created by jordan on 2022-03-08.
//

#ifndef CPT_CPT_SERVER_H
#define CPT_CPT_SERVER_H

#include "tcp_server.h"

#include "cpt_packet_builder.h"
#include "cpt_serialize.h"
#include "cpt_channel.h"
#include "cpt_parse.h"
#include "cpt_user.h"


/**
 * Send serialized server response to client.
 *
 * Serializes a CptResponse object and sends
 * it to a connected client.
 *
 * @param user      Pointer to a User object.
 * @param response  Pointer to a CptResponse object.
 * @return          0 if successful, error code on failure.
 */
int cpt_send_response(CptResponse * response);


/**
 * Handle a received 'LOGIN' protocol message.
 *
 * Use information in the CptPacket to handle
 * a LOGIN protocol message from a connected client.
 * If successful, will accept the tcp connection and
 * push a new User object into the GlobalChannel.
 *
 * @param gc            The GlobalChannel (pointer to a Channel)
 * @param cpt_packet    Pointer to a CptPacket object.
 * @return              0 if successful, error code on failure.
 */
int cpt_handle_login(Channel * gc, CptPacket * packet, int id);


/**
 * Handle a received 'SEND' protocol message.
 *
 * Uses information in the CptPacket to handle
 * a SEND protocol message from a connected client.
 * If successful, will accept the tcp connection and
 * push a new User object into the GlobalChannel.
 *
 * @param gc            The GlobalChannel (pointer to a Channel)
 * @param cpt_packet    Pointer to a CptPacket object.
 * @return              0 if successful, error code on failure.
 */
uint8_t * cpt_msg_response(CptPacket * packet, CptResponse * res, int * result);


/**
 * Handle a received 'GET_USERS' protocol message.
 *
 * Uses information in the CptPacket to handle
 * a SEND a list of users back to the requesting
 * client in the format:
 *      < user_id >< whitespace >< username >< newline >
 *
 * Example given:
 *      1 'Clark Kent'
 *      2 'Bruce Wayne'
 *      3 'Fakey McFakerson'
 *
 * @param dir       A linked list of Channel objects.
 * @param user      User making request.
 * @param packet    Packet sent by requesting user.
 * @return 0 on success, error code on failure.
 */
uint8_t * cpt_get_users_response(Channels dir, CptPacket *packet, CptResponse *res);


/**
 * Handle a received 'CREATE_CHANNEL' protocol message.
 *
 * Handles a CREATE_CHANNEL protocol message from a connected
 * client. Will check the CptPacket msg body for a list of
 * requested user IDs. If no User IDs have been requested,
 * A new channel is created containing only the client user
 * that made the request.
 *
 * @param gc        The GlobalChannel.
 * @param dir       The ChannelDirectory (Pointer to a LinkedList of Channel(s))
 * @param user      The client User object who made the request.
 * @param packet    The Received client packet.
 * @return 0 on success, error code on failure.
 */
int cpt_handle_create_channel(Channel * gc, Channels dir, User * user, CptPacket * packet);


/**
 * Handle a received 'JOIN_CHANNEL' protocol message.
 *
 * Handles a JOIN_CHANNEL request, pushing the requesting
 * user into the channel specified by the CHANNEL_ID
 * in the CptPacket.
 *
 * @param dir       The ChannelDirectory (Pointer to a LinkedList of Channel(s))
 * @param user      The client User object who made the request.
 * @param packet    The Received client packet.
 * @return 0 on success, error code on failure.
 */
int cpt_handle_join_channel(Channels dir, User * user, CptPacket * packet);


/**
 * Handle a received 'LEAVE_CHANNEL' protocol message.
 *
 * Handles a LEAVE_CHANNEL request, checking if the requesting user
 * exists, in the channel, before deleting the corresponding UserNode
 * in the channel.
 *
 * @param dir       The ChannelDirectory (Pointer to a LinkedList of Channel(s))
 * @param user      The client User object who made the request.
 * @param packet    The Received client packet.
 * @return 0 on success, error code on failure.
 */
int cpt_handle_leave_channel(Channels dir, User * user, CptPacket * packet);


/**
 * Generate a simple response message for client.
 *
 * Creates a serialized response packet containing
 * generic messages for operation success or failure.
 *
 * @param res   Pointer to CptResponse object.
 * @return      Pointer to Serialized CptResponse object.
 */
uint8_t * cpt_basic_response(CptResponse * res);

#endif //CPT_CPT_SERVER_H
