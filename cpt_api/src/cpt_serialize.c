//
// Created by jordan on 2022-03-18.
//

#include "cpt_serialize.h"


size_t cpt_serialize_packet(CptRequest * cpt, uint8_t * buffer)
{
    size_t serial_size;

    serial_size = serialize(
            buffer, SERIAL_REQ_FMT,
            cpt->version, cpt->command, cpt->channel_id,
            cpt->msg
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


size_t cpt_serialize_msg(CptMsgSubPacket * msg_response, uint8_t * buffer) // !
{
    size_t serial_size;

    serial_size = serialize(
            buffer, SERIAL_MSG_RES_FMT,
            msg_response->channel_id, msg_response->user_id,
            msg_response->msg_len, msg_response->msg
    );

    return serial_size;
}
