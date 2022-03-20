//
// Created by jordan on 2022-03-18.
//

#include "cpt_parse.h"

void cpt_parse_response(CptResponse * response, uint8_t * buffer)
{
    parse( // !
        buffer, PARSE_RES_FMT,
        &response->code, &response->buffer
    );
}


CptPacket * cpt_parse_packet(uint8_t * packet)
{
    CptPacket * cpt;
    cpt = cpt_packet_init();
    memset(cpt, 0, sizeof(struct cpt_packet));
    char msg_buff[MD_BUFF_SIZE];


    parse( // !
            packet, PARSE_BUILD_FMT,
            &cpt->version, &cpt->command, &cpt->channel_id,
            &cpt->msg_len, msg_buff
    );

    cpt->msg = (uint8_t *)strdup(msg_buff);

    return cpt;
}


uint16_t cpt_parse_channel_query(CptPacket * packet, uint16_t * id_buffer)
{
    uint16_t id;
    int i, id_count;
    uint16_t * query;
    uint8_t * body, len;
    uint16_t IDs[SM_BUFF_SIZE];

    id_count = 0;
    body = packet->msg;
    len = packet->msg_len;
    for (i = 0; i < len; i += 3) /* body is uint8_t, so two bytes give us an ID, skip one for space */
    {
        id = (body[i] + body[i + 1]);
        IDs[id_count++] = id;
    }

    memmove(id_buffer, IDs, sizeof(uint16_t) * id_count);
    return id_count;
}
