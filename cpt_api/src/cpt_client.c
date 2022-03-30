//
// Created by jordan on 2022-03-08.
//

#include "cpt_client.h"

size_t cpt_login(void * cpt, uint8_t * serial_buf, char * name)
{
    uint8_t serial_size;
    CptClientInfo * client_info;
    client_info = (CptClientInfo *)cpt;

    client_info->packet = cpt_request_init();
    cpt_request_chan(client_info->packet, (uint16_t) CHANNEL_ZERO);
    cpt_request_version(client_info->packet, VER_MAJ_LAT, VER_MIN_LAT);
    cpt_request_cmd(client_info->packet, (uint8_t) LOGIN);

    ( name )
        ? cpt_request_msg(client_info->packet, name)
        : cpt_request_msg(client_info->packet, DEFAULT_USERNAME);

    serial_size = cpt_serialize_packet(client_info->packet, serial_buf);
    return serial_size;
}


size_t cpt_logout(void * client_info, uint8_t * serial_buf)
{
    size_t serial_size;
    CptClientInfo * info;

    info = (CptClientInfo *) client_info;

    cpt_request_cmd(info->packet, (uint8_t) LOGOUT);
    serial_size = cpt_serialize_packet(info->packet, serial_buf);

    cpt_request_reset(info->packet);
    return serial_size;
}


size_t cpt_get_users(void * cpt, uint8_t * serial_buf, uint16_t channel_id)
{
    CptRequest * packet;
    size_t serial_size;
    CptClientInfo * client_info;

    client_info = (CptClientInfo *)cpt;
    packet = client_info->packet;

    cpt_request_cmd(packet, (uint8_t) GET_USERS);
    cpt_request_msg(packet, "GET_USERS");
    if (channel_id >= CHANNEL_ZERO)
    {
        cpt_request_chan(packet, channel_id);
        serial_size = cpt_serialize_packet(packet, serial_buf);
        return serial_size;
    } else { return 0; }
}


int cpt_send(void * client_info, uint8_t * serial_buf, char * msg)
{
    uint8_t serial_size;
    CptClientInfo * info;

    info = (CptClientInfo *) client_info;
    info->packet = cpt_request_init();
    cpt_request_chan(info->packet, info->channel);
    cpt_request_version(info->packet, VER_MAJ_LAT, VER_MIN_LAT);
    cpt_request_cmd(info->packet, (uint8_t) SEND);
    cpt_request_msg(info->packet, msg);

    serial_size = cpt_serialize_packet(info->packet, serial_buf);
    return serial_size;
}


size_t cpt_join_channel(void * client_info, uint8_t * serial_buf, uint16_t channel_id)
{
    uint8_t serial_size;
    CptClientInfo * info;

    info = (CptClientInfo *) client_info;

    info->packet = cpt_request_init();
    cpt_request_chan(info->packet, channel_id);
    cpt_request_version(info->packet, VER_MAJ_LAT, VER_MIN_LAT);
    cpt_request_cmd(info->packet, (uint8_t) JOIN_CHANNEL);
    cpt_request_msg(info->packet, "join channel");
    serial_size = cpt_serialize_packet(info->packet, serial_buf);
    cpt_request_reset(info->packet);

    return serial_size;
}


size_t cpt_create_channel(void * cpt, uint8_t * serial_buf, char * user_list)
{
    uint8_t serial_size;
    CptClientInfo * client_info;

    client_info = (CptClientInfo *) cpt;
    client_info->packet = cpt_request_init();
    cpt_request_chan(client_info->packet, (uint16_t) CHANNEL_ZERO);
    cpt_request_version(client_info->packet, VER_MAJ_LAT, VER_MIN_LAT);
    cpt_request_cmd(client_info->packet, (uint8_t) CREATE_CHANNEL);

    if ( user_list )
    {
        cpt_request_msg(client_info->packet, (char *) user_list);
    }
    serial_size = cpt_serialize_packet(client_info->packet, serial_buf);
    return serial_size;
}


size_t cpt_leave_channel(void * cpt, uint8_t * serial_buf, uint16_t channel_id)
{
    uint8_t serial_size;
    CptClientInfo * client_info;

    client_info = (CptClientInfo *) cpt;

    if ( channel_id == CHANNEL_ZERO ) { return SYS_CALL_FAIL; }

    client_info->packet = cpt_request_init();
    cpt_request_chan(client_info->packet, channel_id);
    cpt_request_version(client_info->packet, VER_MAJ_LAT, VER_MIN_LAT);
    cpt_request_cmd(client_info->packet, (uint8_t) LEAVE_CHANNEL);
    cpt_request_msg(client_info->packet, "LEAVE_CHANNEL");

    serial_size = cpt_serialize_packet(client_info->packet, serial_buf);
    return serial_size;
}


CptClientInfo * cpt_init_client_info(char * port, char * ip)
{
    int global_id;
    LinkedList * channels;
    CptClientInfo * client_info;

    global_id = CHANNEL_ZERO;
    if ( !(client_info = malloc(sizeof(struct cpt_client_info))) )
    {
        char buf[SM_BUFF_SIZE];
        snprintf(buf, SM_BUFF_SIZE, "Failed to allocate %zu bytes...", sizeof(struct cpt_client_info));
        write(STDERR_FILENO, buf, strlen(buf));
        return NULL;
    }

    if ( !(client_info->packet = cpt_request_init()) ) { return NULL; }
    client_info->channel = global_id;
    client_info->port = strdup(port); // !
    client_info->ip = strdup(ip);

    channels = init_list_data(&global_id, sizeof(uint16_t));
    client_info->channels = channels;

    return client_info;
}


void cpt_destroy_client_info(CptClientInfo * client_info)
{
    if ( client_info )
    {
        if ( client_info->packet )
        {
            cpt_request_destroy(client_info->packet);
        }
        if ( client_info->ip )
        {
            free(client_info->ip);
            client_info->ip = NULL;
        }
        if ( client_info->port )
        {
            free(client_info->port);
            client_info->port = NULL;
        }

        if ( client_info->name )
        {
            free(client_info->name);
            client_info->name = NULL;
        }

        free(client_info);
        client_info = NULL;
    }
}


bool compare_channels(const uint16_t * channel_id, const uint16_t * target_channel)
{
    return *channel_id == *target_channel;
}

