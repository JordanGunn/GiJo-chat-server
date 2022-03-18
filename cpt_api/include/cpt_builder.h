//
// Created by jordan on 2022-03-08.
//

#ifndef CPT_CPT_BUILDER_H
#define CPT_CPT_BUILDER_H
#include "../../common.h"
#include "cpt_types.h"
#include "serialize.h"


/**
* Initialize cpt struct.
*
* Dynamically allocates a cpt struct and
* initializes all fields.
*
* @return Pointer to cpt struct.
*/
CptBuilder * cpt_builder_init(void);


/**
* Free all memory and set fields to null.
*
* @param cpt   Pointer to a cpt structure.
*/
void cpt_builder_destroy(CptBuilder * cpt);


/**
* Set the command value for the cpt header block.
*
* @param cpt   Pointer to a cpt structure.
* @param cmd   From enum commands.
*/
void cpt_builder_cmd(CptBuilder * cpt, uint8_t cmd);


/**
* Set major and minor version for the cpt header block.
*
* @param cpt           Pointer to a cpt structure.
* @param version_major From enum version.
* @param version_minor From enum version.
*/
void cpt_builder_version(CptBuilder * cpt, uint8_t version_major, uint8_t version_minor);


/**
* Set the message length for the cpt header block.
*
* @param cpt       Pointer to a cpt structure.
* @param msg_len   An 8-bit integer.
*/
void cpt_builder_len(CptBuilder * cpt, uint8_t msg_len);


/**
* Set the channel id for the cpt header block.
*
* @param cpt           Pointer to a cpt structure.
* @param channel_id    A 16-bit integer.
*/
void cpt_builder_chan(CptBuilder * cpt, uint16_t channel_id);


/**
* Set the MSG field for the cpt packet and
* appropriately update the MSG_LEN
*
* @param cpt  Pointer to a cpt structure.
* @param msg  Pointer to an array of characters.
*/
void cpt_builder_msg(CptBuilder * cpt, char * msg);

/**
* Create a cpt struct from a cpt packet.
*
* @param packet    A serialized cpt protocol message.
* @return          A pointer to a cpt struct.
*/
CptBuilder * cpt_builder_parse(uint8_t * packet);


/**
* Create a cpt struct from a cpt packet.
*
* @param packet    A serialized cpt protocol message.
* @return          A pointer to a cpt struct.
*/
size_t cpt_builder_serialize(CptBuilder * cpt, uint8_t * buffer);


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
char * cpt_to_string(CptBuilder * cpt);


/**
 * Reset builder parameters.
 *
 * Reset the builder parameters,
 * and free memory for certain params.
 *
 * @param cpt    A CptBuilder struct.
*/
void cpt_builder_reset(CptBuilder * cpt);


#endif //CPT_CPT_BUILDER_H
