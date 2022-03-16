//
// Created by jordan on 2022-03-08.
//

#ifndef GIJO_CPT_BUILDER_H
#define GIJO_CPT_BUILDER_H

enum msg_type {
    CPT_TEXT,
    CPT_VOICE,
    CPT_IMG
};

enum commands {
    SEND,
    LOGOUT,
    LOGIN,
    GET_USERS,
    CREATE_CHANNEL,
    JOIN_CHANNEL,
    LEAVE_CHANNEL
};

enum version {
    MAJOR_1 = 1,
    MINOR_0 = 0,
    MINOR_1 = 1
};

struct cpt_builder {
    uint8_t  version;
    uint8_t  command ;
    uint16_t channel_id;
    uint8_t  msg_len;
    char *   msg;
};


/**
* Initialize cpt struct.
*
* Dynamically allocates a cpt struct and
* initializes all fields.
*
* @return Pointer to cpt struct.
*/
struct cpt * cpt_builder_init(void);


/**
* Free all memory and set fields to null.
*
* @param cpt   Pointer to a cpt structure.
*/
void cpt_builder_destroy(struct cpt * cpt);


/**
* Set the command value for the cpt header block.
*
* @param cpt   Pointer to a cpt structure.
* @param cmd   From enum commands.
*/
void cpt_builder_cmd(struct cpt * cpt, commands_client cmd);


/**
* Set major and minor version for the cpt header block.
*
* @param cpt           Pointer to a cpt structure.
* @param version_major From enum version.
* @param version_minor From enum version.
*/
void cpt_builder_version(struct cpt * cpt, version version_major, version version_minor);


/**
* Set the message length for the cpt header block.
*
* @param cpt       Pointer to a cpt structure.
* @param msg_len   An 8-bit integer.
*/
void cpt_builder_len(struct cpt * cpt, uint8_t msg_len);


/**
* Set the channel id for the cpt header block.
*
* @param cpt           Pointer to a cpt structure.
* @param channel_id    A 16-bit integer.
*/
void cpt_builder_chan(struct cpt * cpt, uint16_t channel_id);


/**
* Set the MSG field for the cpt packet and
* appropriately update the MSG_LEN
*
* @param cpt  Pointer to a cpt structure.
* @param msg  Pointer to an array of characters.
*/
void cpt_builder_msg(struct cpt * cpt, char * msg);


/**
* Create a cpt struct from a cpt packet.
*
* @param packet    A serialized cpt protocol message.
* @return          A pointer to a cpt struct.
*/
struct cpt * cpt_builder_parse(void * packet);


/**
* Create a cpt struct from a cpt packet.
*
* @param packet    A serialized cpt protocol message.
* @return          A pointer to a cpt struct.
*/
void * cpt_builder_serialize(struct cpt * cpt);


/**
* Check serialized cpt to see if it is a valid cpt block.
*
* @param packet    A serialized cpt protocol message.
* @return          0 if no issues, otherwise CPT error code.
*/
int cpt_validate(void * packet);

#endif //GIJO_CPT_BUILDER_H
