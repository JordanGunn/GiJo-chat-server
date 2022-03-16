//
// Created by jordan on 2022-03-08.
//


#ifndef CPT_CPT_BUILDER_H
#define CPT_CPT_BUILDER_H
#include "../../common.h"
#include "cpt_types.h"

/**
* Initialize cpt struct.
*
* Dynamically allocates a cpt struct and
* initializes all fields.
*
* @return Pointer to cpt struct.
*/
cpt_builder * cpt_builder_init(void);


/**
* Free all memory and set fields to null.
*
* @param cpt   Pointer to a cpt structure.
*/
void cpt_builder_destroy(cpt_builder * cpt);


/**
* Set the command value for the cpt header block.
*
* @param cpt   Pointer to a cpt structure.
* @param cmd   From enum commands.
*/
void cpt_builder_cmd(cpt_builder * cpt, uint8_t cmd);


/**
* Set major and minor version for the cpt header block.
*
* @param cpt           Pointer to a cpt structure.
* @param version_major From enum version.
* @param version_minor From enum version.
*/
void cpt_builder_version(cpt_builder * cpt, uint8_t version_major, uint8_t version_minor);


/**
* Set the message length for the cpt header block.
*
* @param cpt       Pointer to a cpt structure.
* @param msg_len   An 8-bit integer.
*/
void cpt_builder_len(cpt_builder * cpt, uint8_t msg_len);


/**
* Set the channel id for the cpt header block.
*
* @param cpt           Pointer to a cpt structure.
* @param channel_id    A 16-bit integer.
*/
void cpt_builder_chan(cpt_builder * cpt, uint16_t channel_id);


/**
* Set the MSG field for the cpt packet and
* appropriately update the MSG_LEN
*
* @param cpt  Pointer to a cpt structure.
* @param msg  Pointer to an array of characters.
*/
void cpt_builder_msg(cpt_builder * cpt, const char * msg);


/**
* Create a cpt struct from a cpt packet.
*
* @param packet    A serialized cpt protocol message.
* @return          A pointer to a cpt struct.
*/
cpt_builder * cpt_builder_parse(void * packet);


/**
* Create a cpt struct from a cpt packet.
*
* @param packet    A serialized cpt protocol message.
* @return          A pointer to a cpt struct.
*/
size_t cpt_builder_serialize(cpt_builder * cpt, uint8_t * buffer);


/**
* Check serialized cpt to see if it is a valid cpt block.
*
* @param packet    A serialized cpt protocol message.
* @return          0 if no issues, otherwise CPT error code.
*/
int cpt_validate(void * packet);


/**
* Check serialized cpt to see if it is a valid cpt block.
*
* @param packet    A serialized cpt protocol message.
* @return          0 if no issues, otherwise CPT error code.
*/
void cpt_to_string(cpt_builder * cpt);


static uint16_t serialize(uint8_t * buffer, const char * format, ...);
static void parse(uint8_t * buffer, const char * format, ...);
static uint16_t unpack_uint16t(const uint8_t * serial_buffer);
static void pack_uint16(uint8_t * serial_buffer, uint16_t int_16);

#endif //CPT_CPT_BUILDER_H
