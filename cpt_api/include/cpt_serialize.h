//
// Created by jordan on 2022-03-18.
//

#ifndef CPT_CPT_SERIALIZE_H
#define CPT_CPT_SERIALIZE_H

#include "cpt_types.h"
#include "serialize.h"
#include "cpt_definitions.h"


/**
* Serialize a CptPacket struct for transmission.
*
* @param cpt    A CptPacket struct.
* @return       Size of the serialized packet.
*/
size_t cpt_serialize_packet(CptPacket * cpt, uint8_t * buffer);


/**
* Serialize a CptResponse object for transmission.
*
* @param cpt    A CptResponse object.
* @return       Size of the serialized packet.
*/
size_t cpt_serialize_response(CptResponse * res, uint8_t * buffer);


/**
* Serialize a CptMsgResponse response sub-packet object.
*
* @param cpt    A CptResponse object.
* @return       Size of the serialized packet.
*/
size_t cpt_serialize_msg(CptMsgResponse * response, uint8_t * buffer);


#endif //CPT_CPT_SERIALIZE_H
