//
// Created by jordan on 2022-03-08.
//

#include "cpt_client.h"

int cpt_login(void * cpt, char * name)
{
    int fd;
    uint8_t buffer[LG_BUFF_SIZE];
    CptPacket * packet;
    CptClientInfo * client_info;
    client_info = (CptClientInfo *)cpt;
    client_info->fd = -1;                           // if user connects this will overwrite

    packet = cpt_packet_init();
    packet->channel_id = CHANNEL_ZERO;
    cpt_packet_version(packet, VER_MAJOR_LATEST, VER_MINOR_LATEST);
    cpt_packet_cmd(packet, (uint8_t)LOGIN);

    ( name )
        ? cpt_packet_msg(packet, name)
        : cpt_packet_msg(packet, DEFAULT_USER_NAME);

    cpt_serialize_packet(packet, buffer);
    CptPacket * p = cpt_parse_packet(buffer);
    fd = tcp_init_client(client_info->ip, client_info->port);

    if ( fd < 0 )
    {
        const char * message = "Failed to Login to chat...";
        write(STDERR_FILENO, message, strlen(message));
        return EXIT_FAILURE;
    } else { client_info->fd = fd; }

    tcp_client_send(fd, buffer);
    cpt_packet_reset(packet);

    client_info->packet = packet; // save the packet for later.
    return 0;
}


int cpt_get_users(void * cpt, char * query_string)
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

    cpt_packet_cmd(packet, (uint8_t) GET_USERS);
    if (query_string) { cpt_packet_msg(packet, query_string); }
    cpt_serialize_packet(packet, buffer);

    tcp_client_send(client_info->fd, buffer);
    cpt_packet_reset(packet);
    return 0;
}



int cpt_send_msg(void * cpt, char * msg)
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

    cpt_packet_cmd(packet, (uint8_t) SEND);
    cpt_packet_msg(packet, msg);
    cpt_serialize_packet(packet, buffer);

    tcp_client_send(client_info->fd, buffer);
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
    cpt_serialize_packet(packet, buffer);

    tcp_client_send(client_info->fd, buffer);
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
    cpt_serialize_packet(packet, req_buffer);

    tcp_client_send(client_info->fd, req_buffer);
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
    cpt_serialize_packet(packet, req_buffer);

    tcp_client_send(client_info->fd, req_buffer);
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
    cpt_serialize_packet(packet, req_buffer);

    tcp_client_send(client_info->fd, req_buffer);
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

