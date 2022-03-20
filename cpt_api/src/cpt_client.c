//
// Created by jordan on 2022-03-08.
//

#include "cpt_client.h"

uint8_t * cpt_login(void * cpt, char * name)
{
    uint8_t * serial;
    uint8_t serial_size;
    uint8_t buffer[LG_BUFF_SIZE] = {0};
    CptClientInfo * client_info;
    client_info = (CptClientInfo *)cpt;

    client_info->packet = cpt_packet_init();
    cpt_packet_chan(client_info->packet, (uint16_t) CHANNEL_ZERO);
    cpt_packet_version(client_info->packet, VER_MAJOR_LATEST, VER_MINOR_LATEST);
    cpt_packet_cmd(client_info->packet, (uint8_t)LOGIN);

    ( name )
        ? cpt_packet_msg(client_info->packet, name)
        : cpt_packet_msg(client_info->packet, DEFAULT_USER_NAME);

    serial_size = cpt_serialize_packet(client_info->packet, buffer);
    serial = malloc(serial_size);
    if ( serial )
    {
        memset(serial, 0 , serial_size);
        memmove(serial, buffer, serial_size);
    }

    return serial;
}


int cpt_get_users(void * cpt, char * query_string)
{
    CptPacket * packet;
    size_t serial_size;
    CptClientInfo * client_info;
    uint8_t buffer[LG_BUFF_SIZE];

    client_info = (CptClientInfo *)cpt;
    packet = client_info->packet;

    if ( !(client_info->fd) )
    {
        const char * message = "Not connected!";
        write(STDERR_FILENO, message, strlen(message));
        return EXIT_FAILURE;
    }

    cpt_packet_cmd(packet, (uint8_t) GET_USERS);
    if (query_string) { cpt_packet_msg(packet, query_string); }
    serial_size = cpt_serialize_packet(packet, buffer);

    tcp_client_send(client_info->fd, buffer, serial_size);
    cpt_packet_reset(packet);
    return 0;
}



int cpt_send_msg(void * cpt, char * msg)
{
    size_t serial_size;
    CptPacket * packet;
    CptClientInfo * client_info;
    uint8_t buffer[LG_BUFF_SIZE];

    client_info = (CptClientInfo *)cpt;
    packet = client_info->packet;

    if ( !(client_info->fd) )
    {
        const char * message = "Not connected!";
        write(STDERR_FILENO, message, strlen(message));
        return EXIT_FAILURE;
    }

    cpt_packet_cmd(packet, (uint8_t) SEND);
    cpt_packet_msg(packet, msg);
    serial_size = cpt_serialize_packet(packet, buffer);

    tcp_client_send(client_info->fd, buffer, serial_size);
    cpt_packet_reset(packet);

    cpt_packet_reset(packet);

    return 0;
}


int cpt_logout(void * cpt)
{

    CptPacket * packet;
    CptClientInfo * client_info;
    uint8_t buffer[LG_BUFF_SIZE];

    client_info = (CptClientInfo *)cpt;
    packet = client_info->packet;

    if ( !(client_info->fd) )
    {
        const char * message = "Not connected!";
        write(STDERR_FILENO, message, strlen(message));
        return EXIT_FAILURE;
    }

    cpt_packet_cmd(packet, (uint8_t) LOGOUT);
    size_t serial_size;
    serial_size = cpt_serialize_packet(packet, buffer);

    tcp_client_send(client_info->fd, buffer, serial_size);
    cpt_packet_reset(packet);

    return 0;
}


int cpt_join_channel(void * cpt, int channel_id)
{
    CptResponse response;
    CptPacket * packet;
    uint8_t * response_msg;
    CptClientInfo * client_info;
    uint8_t req_buffer[LG_BUFF_SIZE];

    client_info = (CptClientInfo *)cpt;
    packet = client_info->packet;
    if ( !(client_info->fd) )
    {
        const char * message = "Not connected!";
        write(STDERR_FILENO, message, strlen(message));
        return EXIT_FAILURE;
    }

    cpt_packet_cmd(packet, (uint8_t) JOIN_CHANNEL);
    size_t serial_size;
    serial_size = cpt_serialize_packet(packet, req_buffer);

    tcp_client_send(client_info->fd, req_buffer, serial_size);
    response_msg = (uint8_t *)tcp_client_recv(client_info->fd);
    cpt_parse_response(&response, response_msg);

    cpt_packet_reset(packet);
    if (response.code == (uint8_t)SUCCESS)
    {
        client_info->channel = channel_id;
    } else { return -1; }

    return 0;
}


int cpt_create_channel(void * cpt, char * members, bool is_private)
{
    size_t serial_size;
    CptResponse response;
    CptPacket * packet;
    LinkedList * channels;
    uint8_t * response_msg;
    CptClientInfo * client_info;
    uint8_t req_buffer[LG_BUFF_SIZE];

    if ( is_private ) { puts("Handle private channel"); } // !
    client_info = (CptClientInfo *)cpt;
    channels = client_info->channels;
    packet = client_info->packet;
    if ( !(client_info->fd) )
    {
        const char * message = "Not connected!";
        write(STDERR_FILENO, message, strlen(message));
        return EXIT_FAILURE;
    }

    cpt_packet_cmd(packet, (uint8_t) CREATE_CHANNEL);
    if ( members ) { cpt_packet_msg(packet, members); }
    serial_size = cpt_serialize_packet(packet, req_buffer);

    tcp_client_send(client_info->fd, req_buffer, serial_size);
    response_msg = (uint8_t *)tcp_client_recv(client_info->fd);
    cpt_parse_response(&response, response_msg);

    cpt_packet_reset(packet);
    if ( response.code == (uint8_t) SUCCESS )
    {
        uint16_t _id = strtol( // !
                (char *)response.buffer,
                (char **)&response.buffer, BASE_10
            );
        push_node(channels, &_id, channels->node_size);
    } else { return -1; }

    return 0;
}


int cpt_leave_channel(void * cpt, int channel_id)
{
    size_t serial_size;
    CptResponse response;
    CptPacket * packet;
    uint8_t * response_msg;
    CptClientInfo * client_info;
    uint8_t req_buffer[LG_BUFF_SIZE];

    client_info = (CptClientInfo *)cpt;
    packet = client_info->packet;
    if ( !(client_info->fd) )
    {
        const char * message = "Not connected!";
        write(STDERR_FILENO, message, strlen(message));
        return EXIT_FAILURE;
    }

    cpt_packet_cmd(packet, (uint8_t) LEAVE_CHANNEL);
    serial_size = cpt_serialize_packet(packet, req_buffer);

    tcp_client_send(client_info->fd, req_buffer, serial_size);
    response_msg = (uint8_t *)tcp_client_recv(client_info->fd);
    cpt_parse_response(&response, response_msg);

    cpt_packet_reset(packet);
    delete_node(
            client_info->channels,
            (Comparator) compare_channels,
            &channel_id
        ); // !

    return 0;
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

    if ( !(client_info->packet = cpt_packet_init()) ) { return NULL; }
    client_info->channel = global_id;
    client_info->port = strdup(port); // !
    client_info->ip = strdup(ip);

    channels = init_list(&global_id, sizeof(uint16_t));
    client_info->channels = channels;

    return client_info;
}


void cpt_destroy_client_info(CptClientInfo * client_info)
{
    if ( client_info )
    {
        if ( client_info->packet )
        {
            cpt_packet_destroy(client_info->packet);
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
        free(client_info);
        client_info = NULL;
    }
}


bool compare_channels(const uint16_t * channel_id, const uint16_t * target_channel)
{
    return *channel_id == *target_channel;
}

