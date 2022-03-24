//
// Created by jordan on 2022-03-18.
//

#include "cpt_serialize.h"


size_t cpt_serialize_packet(CptPacket * cpt, uint8_t * buffer)
{
    size_t serial_size;

    serial_size = serialize(
            buffer, SERIAL_PACKET_FMT,
            cpt->version, cpt->command, cpt->channel_id,
            cpt->msg_len, cpt->msg
    );

    return serial_size;
}


size_t cpt_serialize_response(CptResponse * res, uint8_t * buffer)
{
    size_t serial_size;

    serial_size = serialize(
            buffer, SERIAL_RES_FMT,
            res->code, res->data
    );

    return serial_size;
}


size_t cpt_serialize_msg(CptMsgResponse * msg_response, uint8_t * buffer) // !
{
    size_t serial_size;

    serial_size = serialize(
            buffer, SERIAL_MSG_RES_FMT,
            msg_response->channel_id, msg_response->user_id,
            msg_response->msg_len, msg_response->msg
    );

    return serial_size;
}


///**
// * Message response sub-packet.
// *
// * Valid pre-serialized format for server
// * transmission of SEND cpt packets.
// */
//struct cpt_msg_response {
//    uint16_t channel_id;
//    uint16_t user_id;
//    uint16_t msg_len;
//    uint8_t * msg;
//};

