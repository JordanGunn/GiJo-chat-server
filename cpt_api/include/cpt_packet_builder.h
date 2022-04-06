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
 * Initialize CptRequest object.
 *
 * Dynamically allocates a cpt struct and
 * initializes all fields.
 *
 * @return Pointer to cpt struct.
*/
CptRequest * cpt_request_init(void);


/**
 * Free all memory and set fields to null.
 *
 * @param req   Pointer to a cpt structure.
*/
void cpt_request_destroy(CptRequest * req);


/**
 * Set the command value for the cpt header block.
 *
 * @param cpt   Pointer to a cpt structure.
 * @param cmd   From enum commands.
*/
void cpt_request_cmd(CptRequest * cpt, uint8_t cmd);


/**
 * Set major and minor version for the cpt header block.
 *
 * @param cpt           Pointer to a cpt structure.
 * @param version_major From enum version.
 * @param version_minor From enum version.
*/
void cpt_request_version(CptRequest * cpt, uint8_t version_major, uint8_t version_minor);


/**
 * Set the message length for the cpt header block.
 *
 * @param cpt       Pointer to a cpt structure.
 * @param msg_len   An 8-bit integer.
*/
void cpt_request_len(CptRequest * cpt, uint8_t msg_len);


/**
 * Set the channel id for the cpt header block.
 *
 * @param cpt           Pointer to a cpt structure.
 * @param channel_id    A 16-bit integer.
*/
void cpt_request_chan(CptRequest * cpt, uint16_t channel_id);


/**
 * Set the MSG field for the cpt packet.
 *
 * Also appropriately updates the MSG_LEN field.
 *
 * @param cpt  Pointer to a cpt structure.
 * @param msg  Pointer to an array of characters.
*/
void cpt_request_msg(CptRequest * cpt, char * msg);


/**
 * Check serialized cpt to see if it is a valid cpt block.\n\n
 *
 * @param packet    A serialized cpt protocol message.
 * @return 0 if no issues, otherwise CPT error code.
*/
int cpt_validate(void * packet);


/**
 * Convert CptRequest object to a string representation.\n\n
 *
 * Converts a CptRequest object to a string, returning
 * a char pointer to the dynamically allocated memory.
 * If fields are missing from the CptRequest, they will
 * be filled with default values to signify as such.
 *
 * If call fails for any reason, function returns NULL.
 *
 *
 * @param packet    A serialized cpt protocol message.
 * @return Char pointer to dynamically allocated string.
*/
char * cpt_to_string(CptRequest * cpt);


/**
 * Reset packet parameters.
 *
 * Reset the packet parameters,
 * and free memory for certain params.
 *
 * @param packet    A CptRequest struct.
*/
void cpt_request_reset(CptRequest * packet);


// ===================================
// C P T   S E R V E R   P A C K E T S
// ===================================

/**
 * Initialize CptResponse server-side packet.
 *
 * Initializes a CptResponse by initializing data
 * members, and returning a dynamically allocated
 * pointer to a CptResponse struct.
 *
 * @param res_code    Received client-side packet.
 * @return CptResponse object.
 */
CptResponse * cpt_response_init(void);


/**
 * Destroy CptResponse object.
 *
 * Destroys CptResponse object, freeing any allocated memory
 * and setting all pointers to null.
 *
 * @param res  Pointer to a CptResponse object.
 */
void cpt_response_destroy(CptResponse * res);


/**
 * Reset packet parameters.
 *
 * Reset the response parameters,
 * and free memory for certain params.
 *
 * @param res    A CptResponse struct.
*/
void cpt_response_reset(CptResponse * res);



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
CptMsgSubPacket * cpt_msg_sp_init(void);


/**
 * Destroy CptMsgResponse object.
 *
 * Destroys CptMsgResponse object, freeing any allocated memory
 * and setting all pointers to null.
 *
 * @param msg_res  Pointer to a CptResponse object.
 */
void cpt_msg_sp_destroy(CptMsgSubPacket * msg_res);


#endif //CPT_CPT_packet_H
