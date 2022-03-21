//
// Created by jordan on 2022-03-08.
//

#ifndef CPT_CPT_packet_H
#define CPT_CPT_packet_H

#include "common.h"
#include "cpt_types.h"
#include "serialize.h"
#include "linked_list.h"
#include "cpt_definitions.h"

// ==================================
// C P T   C L I E N T   P A C K E T
// ==================================


/**
 * Initialize CptPacket object.
 *
 * Dynamically allocates a cpt struct and
 * initializes all fields.
 *
 * @return Pointer to cpt struct.
*/
CptPacket * cpt_packet_init(void);


/**
 * Free all memory and set fields to null.
 *
 * @param cpt   Pointer to a cpt structure.
*/
void cpt_packet_destroy(CptPacket * cpt);


/**
 * Set the command value for the cpt header block.
 *
 * @param cpt   Pointer to a cpt structure.
 * @param cmd   From enum commands.
*/
void cpt_packet_cmd(CptPacket * cpt, uint8_t cmd);


/**
 * Set major and minor version for the cpt header block.
 *
 * @param cpt           Pointer to a cpt structure.
 * @param version_major From enum version.
 * @param version_minor From enum version.
*/
void cpt_packet_version(CptPacket * cpt, uint8_t version_major, uint8_t version_minor);


/**
 * Set the message length for the cpt header block.
 *
 * @param cpt       Pointer to a cpt structure.
 * @param msg_len   An 8-bit integer.
*/
void cpt_packet_len(CptPacket * cpt, uint8_t msg_len);


/**
 * Set the channel id for the cpt header block.
 *
 * @param cpt           Pointer to a cpt structure.
 * @param channel_id    A 16-bit integer.
*/
void cpt_packet_chan(CptPacket * cpt, uint16_t channel_id);


/**
 * Set the MSG field for the cpt packet.
 *
 * Also appropriately updates the MSG_LEN field.
 *
 * @param cpt  Pointer to a cpt structure.
 * @param msg  Pointer to an array of characters.
*/
void cpt_packet_msg(CptPacket * cpt, char * msg);


/**
 * Check serialized cpt to see if it is a valid cpt block.\n\n
 *
 * @param packet    A serialized cpt protocol message.
 * @return 0 if no issues, otherwise CPT error code.
*/
int cpt_validate(void * packet);


/**
 * Convert CptPacket object to a string representation.\n\n
 *
 * Converts a CptPacket object to a string, returning
 * a char pointer to the dynamically allocated memory.
 * If fields are missing from the CptPacket, they will
 * be filled with default values to signify as such.
 *
 * If call fails for any reason, function returns NULL.
 *
 *
 * @param packet    A serialized cpt protocol message.
 * @return Char pointer to dynamically allocated string.
*/
char * cpt_to_string(CptPacket * cpt);


/**
 * Reset packet parameters.
 *
 * Reset the packet parameters,
 * and free memory for certain params.
 *
 * @param cpt    A CptPacket struct.
*/
void cpt_packet_reset(CptPacket * cpt);


// ===================================
// C P T   S E R V E R   P A C K E T S
// ===================================

/**
 * Initialize CptResponse server-side packet.
 *
 * @param packet    Received client-side packet.
 * @param data      Data being sent to client.
 * @return CptResponse object.
 */
CptResponse * cpt_response_init(uint16_t fd, uint16_t res_code);


/**
 * Destroy CptResponse object.
 *
 * Destroys CptResponse object, freeing any allocated memory
 * and setting all pointers to null.
 *
 * @param response  Pointer to a CptResponse object.
 */
void cpt_response_destroy(CptResponse * response);



// =============================
// C P T   S U B - P A C K E T S
// =============================

/**
 * Initialize CptMsgResponse server-side sub-packet.
 *
 * @param packet    Received client-side packet.
 * @param data      Data being sent to client.
 * @return          CptResponse object.
 */
CptMsgResponse * cpt_msg_response_init(uint8_t * msg, uint16_t chan_id, uint16_t user_id);


/**
 * Destroy CptMsgResponse object.
 *
 * Destroys CptMsgResponse object, freeing any allocated memory
 * and setting all pointers to null.
 *
 * @param msg_res  Pointer to a CptResponse object.
 */
void cpt_msg_response_destroy(CptMsgResponse * msg_res);


#endif //CPT_CPT_packet_H
