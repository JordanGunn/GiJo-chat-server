//
// Created by jordan on 2022-03-18.
//

#include "cpt_parse.h"

CptResponse * cpt_parse_response(uint8_t * data, size_t data_size)
{
    CptResponse * res;
    uint8_t code;
    uint16_t fd;

    parse( // !
        data, PARSE_RES_FMT,
        &fd, &code, data
    );

    res = cpt_response_init(fd, code);
    if ( (res->data = malloc(data_size)) )
    {
        memmove(res->data, data, data_size);
    }

    return res;
}


CptPacket * cpt_parse_packet(uint8_t * req_buf, size_t req_size)
{
    CptPacket * packet;
    packet = cpt_packet_init();
    uint8_t buf[MD_BUFF_SIZE] = {0};
    memset(packet, 0, sizeof(struct cpt_packet));

    parse( // !
        req_buf, PARSE_PACKET_FMT,
        &packet->version, &packet->command,
        &packet->channel_id, &packet->msg_len, buf
    );

    if ( (packet->msg = malloc(req_size)) )
    {
        memset(packet->msg, 0, req_size);
        memmove(packet->msg, buf, req_size);
    }

    return packet;
}


uint16_t cpt_parse_channel_query(CptPacket * packet, uint16_t * id_buffer)
{
    uint16_t id;
    int i, id_count;
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
