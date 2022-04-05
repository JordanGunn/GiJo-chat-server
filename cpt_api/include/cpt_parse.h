//
// Created by jordan on 2022-03-18.
//

#ifndef CPT_CPT_PARSE_H
#define CPT_CPT_PARSE_H

#include "cpt_types.h"
#include "serialize.h"
#include "cpt_definitions.h"
#include "cpt_packet_builder.h"


/**
 * @brief Parse serialized server response.
 *
 * @param response  Address to a CptResponse object.
 * @param buffer    Serialized response from server.
 * @return Pointer to filled CptResponse.
 */
CptResponse * cpt_parse_response(uint8_t * data, size_t data_size);


/**
* Create a cpt struct from a cpt packet.
*
* @param packet    A serialized cpt protocol message.
* @return A pointer to a cpt struct.
*/
CptRequest * cpt_parse_request(uint8_t * req_buf, size_t req_size);

#endif //CPT_CPT_PARSE_H
