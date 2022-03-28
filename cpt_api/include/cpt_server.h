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
    int channel_id;
    CptResponse * res;
    Channels * dir;
    Channel * gc;
};


/**
 * Handle a received 'LOGIN' protocol message.
 *
 * Use information in the CptRequest to handle
 * a LOGIN protocol message from a connected client.
 *
 * If successful, the protocol request will be fulfilled,
 * updating any necessary information contained within
 * <server_info>.
 *
 * @param server_info   Server data structures and information.
 * @param name          Name of user in received Packet MSG field.
 * @return Status Code (SUCCESS if successful, other if failure).
 */
int cpt_login_response(void * server_info, char * name);


/**
 * Handle a received 'LOGOUT' protocol message.
 *
 * Uses information in a received CptRequest to handle
 * a LOGOUT protocol message from a connected client.
 *
 * If successful, will remove any instance of the user
 * specified by the user <id> from the GlobalChannel
 * and any other relevant data structures.
 *
 * @param server_info   Server data structures and information.
 * @return Status Code (SUCCESS if successful, other if failure).
 */
int cpt_logout_response(void * server_info);


/**
 * Handle a received 'LOGOUT' protocol message.
 *
 * Uses information in a received CptRequest to handle
 * a GET_USERS protocol message from a connected client.
 *
 * If successful, the function should collect user information
 * from the channel in the CHAN_ID field of the request packet
 * in the following format.
 *      < user_id >< whitespace >< username >< newline >
 *
 * Example given:
 *      1 'Clark Kent'
 *      2 'Bruce Wayne'
 *      3 'Fakey McFakerson'
 *
 * @param server_info   Server data structures and information.
 * @param channel_id    Target channel ID.
 * @return Status Code (SUCCESS if successful, other if failure).
 */
int cpt_get_users_response(void * server_info, uint16_t channel_id);


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
int cpt_create_channel_response(void * server_info, char * id_list);


/**
 * Handle a received 'JOIN_CHANNEL' protocol message.
 *
 * Uses information in a received CptRequest to handle
 * a JOIN_CHANNEL protocol message from a connected client.
 * If successful, function should add the requesting client
 * user into the channel specified by the CHANNEL_ID field
 * in the CptRequest <channel_id>.
 *
 * @param server_info   Server data structures and information.
 * @param channel_id    Target channel ID.
 * @return Status Code (SUCCESS if successful, other if failure).
 */
int cpt_join_channel_response(void * server_info, uint16_t channel_id);


/**
 * Handle a received 'LEAVE_CHANNEL' protocol message.
 *
 * Use information in the CptRequest to handle
 * a LEAVE_CHANNEL protocol message from a connected client.
 * If successful, will remove any instance of the user
 * specified by the user <id> from the GlobalChannel
 * and any other relevant data structures.
 *
 * @param server_info   Server data structures and information.
 * @param channel_id    Target channel ID.
 * @return Status Code (SUCCESS if successful, other if failure).
 */
int cpt_leave_channel_response(void * server_info, uint16_t channel_id);


///**
// * Handle a received 'SEND' protocol message.
// *
// * Uses information in a received CptRequest to handle
// * a SEND protocol message from a connected client.
// *
// * If successful, function will send the message in the
// * MSG field of the received packet to every user in the
// * CHAN_ID field of the received packet.
// *
// * @param server_info   Server data structures and information.
// * @param channel_id    Target channel for chat message.
// * @return Status Code (0 if successful, other if failure).
// */
//int cpt_send_response(void * server_info, uint16_t channel_id);

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
 * Initialize and allocate all necessary server data structures.
 *
 * Initializes and allocates necessary memory for data structure
 * object pointers.
 *
 * @param gc   Pointer to the GlobalChannel object.
 * @param dir  Pointer to a LinkedList of Channel objects.
 * @return     Pointer to CptServerInfo object.
 */
CptServerInfo * cpt_server_info_init(Channel * gc, Channels * dir);


/**
 * Destroy te ServerInfo object.
 *
 * Free any allocated memory and set pointer to NULL.
 *
 * @param gc   Pointer to the GlobalChannel object.
 * @return     Pointer to CptServerInfo object.
 */
CptServerInfo * cpt_server_info_destroy(CptServerInfo * server_info);


#endif //CPT_CPT_SERVER_H
