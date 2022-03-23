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


typedef struct cpt_server_info CptServerInfo;


struct cpt_server_info
{
    int current_id;
    Channels * dir;
    Channel * gc;
};


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
int cpt_login_response(Channel * gc, CptPacket * packet, int id);



/**
 * Handle a received 'LOGOUT' protocol message.
 *
 * Use information in the CptPacket to handle
 * a LOGOUT protocol message from a connected client.
 * If successful, will remove any instance of the user
 * specified by the user <id> from the GlobalChannel
 * and the ChannelDirectory.
 *
 * @param gc        The GlobalChannel (pointer to a Channel)
 * @param dir       A linked list of Channel objects.
 * @param id        User ID.
 * @return Status Code (0 if successful, other if failure).
 */
int cpt_logout_response(Channel *gc, Channels * dir, int id);


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
uint8_t * cpt_get_users_response(Channels * dir, CptPacket * packet, CptResponse * res);


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
int cpt_create_channel_response(Channel * gc, Channels * dir, CptPacket * packet, int id);


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
int cpt_handle_join_channel(Channels * dir, User * user, CptPacket * packet);


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
int cpt_handle_leave_channel(Channels * dir, User * user, CptPacket * packet);



/**
 * Serialize CptResponse object for transmission to client.
 *
 * @param res       Pointer to CptResponse object.
 * @param res_buf   Buffer for serialized CptResponse.
 * @return Size of serialized CptResponse.
 */
size_t cpt_response(CptResponse * res, uint8_t * res_buf);


/**
 * Generate a simple response message for client.
 *
 * Creates a serialized response packet containing
 * generic messages for operation success or failure.
 *
 * @param res   Pointer to CptResponse object.
 * @return      Pointer to Serialized CptResponse object.
 */
size_t cpt_simple_response(CptResponse * res, uint8_t * res_buf);


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
CptServerInfo * cpt_server_info_init(Channel * gc, Channels * dir);


#endif //CPT_CPT_SERVER_H
