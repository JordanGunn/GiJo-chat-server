//
// Created by jordan on 2022-03-18.
//

#ifndef CPT_CPT_DEFINITIONS_H
#define CPT_CPT_DEFINITIONS_H


/**
 * CPT constants definitions.
 */
/* ============================== */
#define DEFAULT_USER_NAME "anonymous"
#define ACCESS_PRIVATE "PRIVATE"
#define ACCESS_PUBLIC "PUBLIC"
#define VERSION_MINOR_MAX 15
#define VERSION_MAJOR_MAX 15
#define VER_MAJOR_LATEST 1
#define VER_MINOR_LATEST 1
#define MAX_NAME_SIZE 12
#define CHANNEL_ZERO 0
/* ============================== */


/**
 * Valid parsing/serialization formats.
 */
/* ============================ */
#define SERIAL_RES_FMT "Cs"
#define PARSE_RES_FMT "C512s"
#define SERIAL_BUILD_FMT "CCHCs"
#define SERIAL_MSG_RES_FMT "HHHs"
#define PARSE_BUILD_FMT "CCH512s"
#define PARSE_MSG_RES_FMT "HHH512s"
/* ============================ */


/**
 * Valid cpt protocol response codes.
 */
/* ===================== */
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
#define BAD_RESPONSE     14
#define BAD_USER         15
#define BAD_PACKET       16
#define BAD_CHANNEL      17
#define NAME_TOO_LONG    18
#define SERVER_ERROR     19
#define MSG_RESPONSE     20
#define MSG_EMPTY        21
/* ===================== */

#endif //CPT_CPT_DEFINITIONS_H
