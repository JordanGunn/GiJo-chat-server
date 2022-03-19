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
struct cpt_packet {
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
struct cpt_client_info {
    int fd;
    char * ip;
    char * port;
    char * name;
    uint16_t channel;
    CptPacket * packet;
    LinkedList * channels;
};


/**
 * Cpt response object.
 *
 * Contains properties for preparing cpt response packets.
 * When ready, can be passed to cpt_parse_response() or
 * cpt_serialize_response()/
 *
 */
struct cpt_response {
    uint8_t code;
    uint8_t * buffer;
};


/**
 * Message response sub-packet.
 *
 * Valid pre-serialized format for server
 * transmission of SEND cpt packets.
 */
struct cpt_msg_response {
    uint16_t channel_id;
    uint16_t user_id;
    uint16_t msg_len;
    uint8_t * msg;
};


/**
 * Username-ID pair format.
 *
 * Valid format for USER_LIST server
 * response sub-packet.
 */
struct username_id_pair {
    uint16_t id;
    uint8_t * username;
};


enum {
    CPT_TEXT,
    CPT_VOICE,
    CPT_IMG
} msg_type;


/**
 * Valid cpt protocol packet commands.
 */
enum {
    SEND,
    LOGOUT,
    LOGIN,
    GET_USERS,
    CREATE_CHANNEL,
    JOIN_CHANNEL,
    LEAVE_CHANNEL
} commands;


/**
 * Valid major and minor numbers for
 * cpt protocol versioning.
 */
enum {
    MAJOR_2 = 2,
    MAJOR_1 = 1,
    MINOR_1 = 1,
    MINOR_0 = 0
} version;

#endif //CPT_CPT_TYPES_H
