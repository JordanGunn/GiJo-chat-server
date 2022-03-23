//
// Created by jordan on 2022-03-08.
//

#ifndef CPT_CPT_CLIENT_H
#define CPT_CPT_CLIENT_H

#include "tcp_client.h"
#include "cpt_parse.h"
#include "cpt_serialize.h"
#include "cpt_definitions.h"
#include "cpt_packet_builder.h"


/**
* Prepare a LOGIN request from a CptPacket.
*
* Prepares client information necessary to be added to the
* server. Successful transmission of this packet should enable
* the client-side user's information to persist within the
* server until a LOGOUT packet is sent.
*
* @param cpt           CPT packet and any additional information.
* @return The size of the serialized packet in <serial_buf>.
*/
size_t cpt_login(void * cpt, uint8_t * serial_buf, char * name);


/**
* Prepare a GET_USERS request for the server.
*
* Prepares a GET_USERS request to the server. If successful,
* the resulting data in <serial_buf> will contain a CPT packet
* with the necessary information to instruct the server to
* send back a list of users specified by the <channel_id>.
*
* @param cpt            CPT packet information and any other necessary data.
* @param serial_buf     A buffer intended for storing the result.
* @param channel_id     The ID of the CHANNEL you wish to get a list
*                       of users from.
* @return Size of the resulting serialized packet in <serial_buf>
*/
size_t cpt_get_users(void * cpt, uint8_t * serial_buf, uint16_t channel_id);


/**
* Prepare a CREATE_CHANNEL request for the server.
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
size_t cpt_create_channel(void * client_info, uint8_t * serial_buf, char * user_list);


/**
* Prepare a LOGOUT request packet for the server.
*
* Sends a server request to remove the client from the server.
* Successful execution of this function call should remove the
* existing client's information from any and all data structures
* used to represent a CHANNEL.
*
* @param cpt            CPT packet information and any other necessary data.
* @param serial_buf     A buffer intended for storing the result.
* @return Size of the resulting serialized packet in <serial_buf>
*/
size_t cpt_logout(void * cpt, uint8_t * serial_buf);


/**
* Prepare a SEND request from a CptPacket.
*
* Prepares a message to the channel specified in the packet CHAN_ID.
*      > <msg> may be passed in optionally in the event that the
*        MSG field of the packet has not been assigned.
*      > If <msg> is not NULL, it will overwrite the
*        existing MSG of the CPT packet and update
*        the MSG_LEN field accordingly.
*      > <msg_flag> may be passed in optionally to specify
*        the type of data being sent to the channel.
*      > If <msg_type> is not NULL, the function will overwrite
*        the existing value of the MSG_TYPE bit fields in the CPT
*        packets to the value passed to <msg_type>.
*      > If <msg_type> is NULL, the function will send a standard
*        text-based message, overwriting the existing flags set
*        in MSG_TYPE with the predefined value of CPT_TEXT.
*
* @param cpt  CPT packet and any additional information.
* @param msg  A string intended to provide additional
*             query params to the server.
* @return A status code. Either from server, or user defined.
*/
int cpt_send_msg(void * cpt, char * msg);


/**
* Add a user to the channel on the server.
*
* Adds a user to an existing channel on the server.
*      > Successful execution will appropriately move
*        the user to the specified CHAN_ID.
*      > Execution of this function will NOT remove
*        the user from any CHANNEL they exist in already.
*      > <channel_id> may be passed in optionally in
*        to join a channel different than the one
*        existing in the cpt packet.
*      > If <channel_id> is not NULL, it will overwrite the
*        the existing CHAN_ID field in the cpt packet.
*
* @param cpt         CPT packet and any additional information.
* @param channel_id  The ID of the intended channel to be removed from.
* @return 		   A status code. Either from server, or user defined.
*/
int cpt_join_channel(void * cpt, int channel_id);


/**
 *
 * Sends a server request to remove the client from the current channel stored in the CHAN_ID field of the transmitted cpt packet.
 * <cpt> must contain a valid cpt packet.
 * The channel number found in the CHAN_ID field of the cpt packet must be a positive integer
 * The sender of the packet must be a member of the channel they intend to leave.
 * Successful execution will remove any instance of the user in the CHANNEL within the server and move them the CHANNEL 0 (zero) by default.
 * <channel_id> may be passed in optionally to leave a channel different than the one existing in the cpt packet.
 * If <channel_id> is not NULL, it will overwrite the existing CHAN_ID field in the cpt packet.
 * An attempt to leave CHANNEL 0 (zero), either by setting the <channel_id> or using the existing CHAN_ID field in the packet is forbidden, and will return an error code.
 *
 * @param cpt           CPT packet and any additional information.
 * @param channel_id    The ID of the intended channel to be removed from.
 * @return A status code. Either from server, or user defined.
 */
int cpt_leave_channel(void * cpt, int channel_id);


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
CptClientInfo * cpt_init_client_info(char * port, char * ip);


/**
 * Destroy CptPacketInfo object.
 *
 * Frees any memory necessary for the
 * object and set all pointers to NULL.
 *
 * @param packet_info CptPacketInfo object.
 */
void cpt_destroy_client_info(CptClientInfo * client_info);


/**
 * A helper for cpt_leave_channel.
 *
 * comparator function pointer that compares
 * channel IDs in a LinkedList object.
 *
 * @param channel_id      A channel ID.
 * @param target_channel  Another channel ID.
 * @return
 */
bool compare_channels(const uint16_t * channel_id, const uint16_t * target_channel);




#endif //CPT_CPT_CLIENT_H
