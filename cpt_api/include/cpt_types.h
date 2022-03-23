//
// Created by jordan on 2022-03-08.
//

#ifndef CPT_CPT_TYPES_H
#define CPT_CPT_TYPES_H

#include "linked_list.h"


typedef struct cpt_packet CptPacket;
typedef struct cpt_response CptResponse;
typedef struct cpt_client_info CptClientInfo;
typedef struct cpt_server_info CptServerInfo;
typedef struct cpt_msg_response CptMsgResponse;
typedef struct username_id_pair UserNameIdPair;


/**
 * Cpt packet object.
 *
 * Used to assign the properties
 * of a cpt packet. Can be passed
 * to cpt_serialize() for transmission
 * to a cpt server.
 *
 */
struct cpt_packet
{
    uint8_t   version;
    uint8_t   command ;
    uint16_t  channel_id;
    uint8_t   msg_len;
    uint8_t * msg;
};


/**
 * Client info object.
 *
 * Holds all useful or relevant information for
 * client side application programming. This
 * includes sending/receiving messages, storing
 * active user channels, etc.
 *
 */
struct cpt_client_info
{
    int fd;
    int code;
    char * ip;
    char * port;
    char * name;
    uint16_t channel;
    CptPacket * packet;
    LinkedList * channels;
};


/**
 * Cpt response object.\n\n
 *
 * Contains properties for preparing cpt response packets.
 * When ready, can be passed to cpt_parse_response() or
 * cpt_serialize_response()/
 *
 */
struct cpt_response
{
    uint16_t fd;
    uint8_t code;
    uint8_t * data;
};


/**
 * Message response sub-packet.\n\n
 *
 * Valid pre-serialized format for server
 * transmission of SEND cpt packets.
 */
struct cpt_msg_response
{
    uint16_t channel_id;
    uint16_t user_id;
    uint16_t msg_len;
    uint8_t * msg;
};


/**
 * Username-ID pair format.\n\n
 *
 * Valid format for USER_LIST server
 * response sub-packet.
 */
struct username_id_pair
{
    uint16_t id;
    uint8_t * username;
};


/**
 * Valid cpt protocol packet commands.
 */
enum commands
{
    SEND = 1,
    LOGOUT = 2,
    GET_USERS,
    CREATE_CHANNEL,
    JOIN_CHANNEL,
    LEAVE_CHANNEL,
    LOGIN
};


/**
 * Valid major and minor numbers for
 * cpt protocol versioning.
 */
enum version
{
    MAJOR_2 = 2,
    MAJOR_1 = 1,
    MINOR_1 = 1,
    MINOR_0 = 0
};

#endif //CPT_CPT_TYPES_H
