//
// Created by jordan on 2022-03-08.
//

#ifndef CPT_CPT_TYPES_H
#define CPT_CPT_TYPES_H

#define CHANNEL_ZERO 0
#define MAX_NAME_SIZE 12
#define ACCESS_PUBLIC "PUBLIC"
#define ACCESS_PRIVATE "PRIVATE"
#define DEFAULT_USER_NAME "anonymous"

typedef struct CptBuilder CptBuilder;
typedef struct cpt_packet_info CptPacketInfo;

struct cpt_builder {
    uint8_t   version;
    uint8_t   command ;
    uint16_t  channel_id;
    uint8_t   msg_len;
    uint8_t * msg;
};

struct cpt_packet_info {
    int fd;
    char * ip;
    char * port;
    CptBuilder * builder;
    uint8_t * serial_buffer;
};

enum {
    CPT_TEXT,
    CPT_VOICE,
    CPT_IMG
} msg_type;

enum {
    SEND,
    LOGOUT,
    LOGIN,
    GET_USERS,
    CREATE_CHANNEL,
    JOIN_CHANNEL,
    LEAVE_CHANNEL
} commands;

enum {
    MAJOR_2 = 2,
    MAJOR_1 = 1,
    MINOR_1 = 1,
    MINOR_0 = 0
} version;

/* SERVER RESPONSE CODES */
#define SUCCESS          0
#define UKNOWN_CMD       1
#define LOGIN_FAIL       2
#define UKNOWN_CHANNEL   3
#define BAD_VERSION      4
#define SEND_FAILED      5
#define CHAN_ID_OVERFLOW 6
#define MSG_OVERFLOW     7
#define MSG_LEN_OVERFLOW 8
#define CHAN_EMPTY       9
#define USER_LEFT_CHAN   10
#define INVALID_ID       11
#define UNAUTH_ACCESS    12
#define SERVER_FULL      13

#define VERSION_MINOR_MAX 15
#define VERSION_MAJOR_MAX 15
#define VERSION_MAJOR_LATEST 1
#define VERSION_MINOR_LATEST 1





#endif //CPT_CPT_TYPES_H
