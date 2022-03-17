//
// Created by jordan on 2022-03-08.
//

#include "../include/cpt_client.h"
#include "../../tcp_networking/include/tcp_client.h"

int cpt_login(void * cpt, char * name)
{
    int fd;
    uint8_t serial_buffer[LG_BUFF_SIZE];
    CptPacketInfo * packet_info;

    packet_info = (CptPacketInfo *)cpt;                  // initial setup
    packet_info->builder = cpt_builder_init();           // initial setup
    packet_info->builder->channel_id = CHANNEL_ZERO;     // initial setup

    cpt_builder_version(packet_info->builder,
            VERSION_MAJOR_LATEST, VERSION_MINOR_LATEST);

    cpt_builder_cmd(packet_info->builder,(uint8_t) LOGIN);

    ( name )
        ? cpt_builder_msg(packet_info->builder, name)
        : cpt_builder_msg(packet_info->builder, DEFAULT_USER_NAME);

    cpt_builder_serialize(packet_info->builder, serial_buffer);
    fd = tcp_init_client(packet_info->ip, packet_info->port);

    if ( fd < 0 )
    {
        const char * message = "Failed to Login to chat...";
        write(STDERR_FILENO, message, strlen(message));
        return EXIT_FAILURE;
    } else { packet_info->fd = fd; }

    tcp_client_send(fd, serial_buffer);
    return 0;
}


int cpt_get_users(void * cpt, char * query_string);



int cpt_send_msg(void * cpt, char * msg, int msg_flag);


int cpt_logout(void * cpt);


int cpt_join_channel(void * cpt, int channel_id);


int cpt_create_channel(void * cpt, void * members, bool is_private);


int cpt_leave_channel(void * cpt, int channel_id);


CptPacketInfo * cpt_init_packet_info(char * port, char * ip)
{
    CptPacketInfo * packet_info;

    packet_info = malloc_safely(sizeof(struct cpt_packet_info), STDERR_FILENO); // !
    if ( !(packet_info->builder = cpt_builder_init()) ) { return NULL; }
    packet_info->port = strdup(port); // !
    packet_info->ip = strdup(ip);

    return packet_info;
}

void cpt_destroy_packet_info(CptPacketInfo * packet_info)
{
    if ( packet_info )
    {
        if ( packet_info->builder ) { cpt_builder_destroy(packet_info->builder); }
        if ( packet_info->ip ) { free(packet_info->ip); packet_info->ip = NULL;  }
        if ( packet_info->ip ) { free(packet_info->port); packet_info->port = NULL; }
        free(packet_info);
        packet_info = NULL;
    }
}