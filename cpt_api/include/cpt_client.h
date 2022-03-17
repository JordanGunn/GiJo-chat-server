//
// Created by jordan on 2022-03-08.
//

#ifndef CPT_CPT_CLIENT_H
#define CPT_CPT_CLIENT_H

#include "../include/cpt_builder.h"
#include "../../tcp_networking/include/tcp_client.h"

typedef struct cpt_packet_info CptPacketInfo;

struct cpt_packet_info {
    int fd;
    cpt_builder * builder;
    uint8_t * serial_buffer;
};

/**
* Send client information to the server.
*
* Sends client information and adds them to the chat server.
* Successful execution of this function call should enable
* the client program's information to persist within the
* server until cpt_logout() is called.
*
* @param cpt   CPT packet and any additional information.
* @return      A status code. Either from server, or user defined.
*/
int cpt_login(void * cpt, char * name);


/**
* Get users from the server.
*
* Makes a request to the CPT server for errors.
*      > <query_string> may be passed in optionally,
*        as additional filter parameters for the server's
*        data organization structure(s).
*      > If <query_string> is not NULL, it will overwrite
*        the existing BODY of the CPT packet before sending.
*
* @param cpt           CPT packet and any additional information.
* @param query_string  A string intended to provide additional
*                      query params to the server.
* @return A status code. Either from server, or user defined.
*/
int cpt_get_users(void * cpt, char * query_string);


/**
* Send a message to the current channel.
*
* Sends a message to the channel specified in the packet CHAN_ID.
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
int cpt_send_msg(void * cpt, char * msg, int msg_flag);


/**
* Remove client information from the server.
*
* Sends a server request to remove the client from the server.
* Successful execution of this function call should remove the
* existing client's information from any and all data structures
* used to represent a CHANNEL.
*
* @param cpt   CPT packet and any additional information.
* @return      A status code. Either from server, or user defined.
*/
int cpt_logout(void * cpt);


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
* Create a new CHANNEL on the server.
*
* Sends a request to the server to create a new channel.
* If successful, the server will create a new CHANNEL data
* structure and add the client to the CHANNEL.
*      > <members> may be optionally passed as selection
*        parameters for the server.
*        If the parameters in <members> are sufficient to
*        identify the target channel members, all members
*        will be added to the channel by default.
*      > If <members> is not NULL, it will overwrite the
*        existing BODY of the cpt packet.
*      > If <is_private> is set to true, the packet ACCESS
*        bit field will be overwritten to PRIVATE making
*        the created CHANNEL visible only to the user, and
*        any members added via the <members> parameter.
*      > If <is_private> is set to false, the packet ACCESS
*        bit field will be overwritten to PUBLIC making the
*        created CHANNEL visible to all active users on the server.
*      > If <is_private> is NULL, the channel will be created using
*        the current FLAG value set in the ACCESS bit field.
*
* @param cpt         CPT packet and any additional information.
* @param members     Additional member selection parameters for the server.
* @param is_private  Set the created channel to private or public.
* @return A status code. Either from server, or user defined.
*/
int cpt_create_channel(void * cpt, void * members, bool is_private);


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


#endif //CPT_CPT_CLIENT_H
