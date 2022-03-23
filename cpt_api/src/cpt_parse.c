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


uint16_t cpt_parse_channel_query(CptPacket * packet, uint16_t * id_buf)
{
    uint8_t * body;
    char * id_str;
    int id_count;
    uint16_t id;


    id_count = 0;
    body = packet->msg;
    while ( strlen((char *) body) )
    {
        id = strtol((char *)body, &id_str, 10);
        body = (uint8_t *) id_str;
        if ( id != 0 )
        {
            if ( id != id_buf[0] )
            { id_buf[id_count++] = id; }
        }
    }
    return id_count;
}
