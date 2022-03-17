//
// Created by jordan on 2022-03-08.
//

#include "../include/cpt_client.h"
#include "../../tcp_networking/include/tcp_client.h"


int cpt_login(void * cpt, char * name)
{
    uint8_t serial_buffer[LG_BUFF_SIZE];
    CptPacketInfo * packet_info;

    packet_info = (CptPacketInfo *)cpt;

    packet_info->serial_buffer = serial_buffer;
    packet_info = (CptPacketInfo *) packet_info;
    packet_info->builder = cpt_builder_init();
    packet_info->builder->channel_id = CHANNEL_ZERO;

    cpt_builder_version(packet_info->builder, VERSION_MAJOR_LATEST, VERSION_MINOR_LATEST);
    cpt_builder_cmd(packet_info->builder, (uint8_t)LOGOUT);
    ( name )
        ? cpt_builder_msg(packet_info->builder, name)
        : cpt_builder_msg(packet_info->builder, DEFAULT_NAME);

    cpt_builder_serialize(packet_info->builder, packet_info->serial_buffer);

}


int cpt_get_users(void * cpt, char * query_string);



int cpt_send_msg(void * cpt, char * msg, int msg_flag);


int cpt_logout(void * cpt);


int cpt_join_channel(void * cpt, int channel_id);


int cpt_create_channel(void * cpt, void * members, bool is_private);


int cpt_leave_channel(void * cpt, int channel_id);