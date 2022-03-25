//
// Created by jordan on 2022-03-08.
//

#ifndef CPT_CPT_SERVER_H
#define CPT_CPT_SERVER_H

#include "tcp_server.h"

#include "cpt_packet_builder.h"
#include "cpt_serialize.h"
#include "channel.h"
#include "cpt_parse.h"
#include "user.h"


typedef struct cpt_server_info CptServerInfo;


struct cpt_server_info
{
    int current_id;
    CptResponse * res;
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
 * If successful, the protocol request will be fulfilled,
 * updating any necessary information contained within
 * <server_info>.
 *
 * @param server_info   Server data structures and information.
 * @param name          Name of user in received Packet MSG field.
 * @return              1 if successful, error code on failure.
 */
int cpt_login_response(void * server_info, char * name);


/**
 * Handle a received 'LOGOUT' protocol message.
 *
 * Use information in the CptPacket to handle
 * a LOGOUT protocol message from a connected client.
 * If successful, will remove any instance of the user
 * specified by the user <id> from the GlobalChannel
 * and any other relevant data structures.
 *
 * @param server_info   Server data structures and information.
 * @return Status Code (0 if successful, other if failure).
 */
int cpt_logout_response(void * server_info);


/**
 *
 * @param gc            The GlobalChannel (pointer to a Channel)
 * @param cpt_packet    Pointer to a CptPacket object.
 * @return              0 if successful, error code on failure.
 */
uint8_t * cpt_msg_response(CptPacket * packet, CptResponse * res, int * result);


/**
 * Handle a received 'GET_USERS' protocol message.
 *
 * Retrieves all users in the specified <chan_id>,
 * placing them in a buffer for the CptResponse packet
 * in the format:
 *      < user_id >< whitespace >< username >< newline >
 *
 * Example given:
 *      1 'Clark Kent'
 *      2 'Bruce Wayne'
 *      3 'Fakey McFakerson'
 *
 * @param server_info   Server data structures and information.
 * @return Status Code (0 if successful, other if failure).
 */
int cpt_get_users_response(void * server_info, int chan_id);

//TODO doxygen needs to be updated.
/**
 * Handle a received 'CREATE_CHANNEL' protocol message.
 *
 * If a <user_list> was received in the GET_CHANNEL body,
 * function will also parse the <user_list> string and attempt
 * to add the requested user IDs to the channel.
 *
 * If <id_list> is NULL, function will create a new channel with
 * only the requesting user within it.
 *
 * @param server_info   Server data structures and information.
 * @return Status Code (0 if successful, other if failure).
 */
int cpt_create_channel_response(void * server_info, char * id_list);


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
int cpt_join_channel_response(void * server_info, uint16_t channel_id);


/**
 * Handle a received 'LEAVE_CHANNEL' protocol message.
 *
 * Use information in the CptPacket to handle
 * a LEAVE_CHANNEL protocol message from a connected client.
 * If successful, will remove any instance of the user
 * specified by the user <id> from the GlobalChannel
 * and any other relevant data structures.
 *
 * @param server_info   Server data structures and information.
 * @param channel_id    Target channel ID.
 * @return Status Code (0 if successful, other if failure).
 */
int cpt_leave_channel_response(void * server_info, uint16_t channel_id);


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
