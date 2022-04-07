//
// Created by jordan on 2022-04-07.
//

#include "cpt_voice.h"

size_t cpt_create_vchannel(void * cpt, uint8_t * serial_buf, char * user_list)
{
    uint16_t serial_size;
    ClientInfo * client_info;

    client_info = (ClientInfo *) cpt;
    client_info->packet = cpt_request_init();
    cpt_request_chan(client_info->packet, (uint16_t) CHANNEL_ZERO);
    cpt_request_version(client_info->packet, VER_MAJ_LAT, VER_MIN_LAT);
    cpt_request_cmd(client_info->packet, (uint8_t) CREATE_CHANNEL);

    if ( user_list )
    {
        cpt_request_msg(client_info->packet, user_list);
    }
    serial_size = (uint16_t) cpt_serialize_packet(client_info->packet, serial_buf);
    return serial_size;
}
