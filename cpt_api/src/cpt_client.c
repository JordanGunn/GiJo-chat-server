//
// Created by jordan on 2022-03-08.
//

#include "../include/cpt_client.h"

int cpt_login(void * cpt, char * name)
{
    int fd;
    uint8_t buffer[LG_BUFF_SIZE];
    CptBuilder * builder;
    CptClientInfo * client_info;
    client_info = (CptClientInfo *)cpt;
    client_info->fd = -1;                           // if user connects this will overwrite

    builder = cpt_builder_init();
    builder->channel_id = CHANNEL_ZERO;
    cpt_builder_version(builder, VER_MAJOR_LATEST, VER_MINOR_LATEST);
    cpt_builder_cmd(builder, (uint8_t)LOGIN);

    ( name )
        ? cpt_builder_msg(builder, name)
        : cpt_builder_msg(builder, DEFAULT_USER_NAME);

    cpt_builder_serialize(builder, buffer);
    fd = tcp_init_client(client_info->ip, client_info->port);

    if ( fd < 0 )
    {
        const char * message = "Failed to Login to chat...";
        write(STDERR_FILENO, message, strlen(message));
        return EXIT_FAILURE;
    } else { client_info->fd = fd; }

    tcp_client_send(fd, buffer);
    cpt_builder_reset(builder);

    client_info->builder = builder; // save the builder for later.
    return 0;
}


int cpt_get_users(void * cpt, char * query_string)
{
    CptBuilder * builder;
    CptClientInfo * client_info;
    uint8_t buffer[LG_BUFF_SIZE];

    client_info = (CptClientInfo *)cpt;
    builder = client_info->builder;

    if ( !(client_info->fd) )
    {
        const char * message = "Not connected!";
        write(STDERR_FILENO, message, strlen(message));
        return EXIT_FAILURE;
    }

    cpt_builder_cmd(builder, (uint8_t) GET_USERS);
    if (query_string) { cpt_builder_msg(builder, query_string); }
    cpt_builder_serialize(builder, buffer);

    tcp_client_send(client_info->fd, buffer);
    cpt_builder_reset(builder);
    return 0;
}



int cpt_send_msg(void * cpt, char * msg)
{
    CptBuilder * builder;
    CptClientInfo * client_info;
    uint8_t buffer[LG_BUFF_SIZE];

    client_info = (CptClientInfo *)cpt;
    builder = client_info->builder;

    if ( !(client_info->fd) )
    {
        const char * message = "Not connected!";
        write(STDERR_FILENO, message, strlen(message));
        return EXIT_FAILURE;
    }

    cpt_builder_cmd(builder, (uint8_t) SEND);
    cpt_builder_msg(builder, msg);
    cpt_builder_serialize(builder, buffer);

    tcp_client_send(client_info->fd, buffer);
    cpt_builder_reset(builder);

    cpt_builder_reset(builder);

    return 0;
}


int cpt_logout(void * cpt)
{
    CptBuilder * builder;
    CptClientInfo * client_info;
    uint8_t buffer[LG_BUFF_SIZE];

    client_info = (CptClientInfo *)cpt;
    builder = client_info->builder;

    if ( !(client_info->fd) )
    {
        const char * message = "Not connected!";
        write(STDERR_FILENO, message, strlen(message));
        return EXIT_FAILURE;
    }

    cpt_builder_cmd(builder, (uint8_t) LOGOUT);
    cpt_builder_serialize(builder, buffer);

    tcp_client_send(client_info->fd, buffer);
    cpt_builder_reset(builder);

    return 0;
}


int cpt_join_channel(void * cpt, int channel_id)
{
    CptResponse response;
    CptBuilder * builder;
    uint8_t * response_msg;
    CptClientInfo * client_info;
    uint8_t req_buffer[LG_BUFF_SIZE];

    client_info = (CptClientInfo *)cpt;
    builder = client_info->builder;
    if ( !(client_info->fd) )
    {
        const char * message = "Not connected!";
        write(STDERR_FILENO, message, strlen(message));
        return EXIT_FAILURE;
    }

    cpt_builder_cmd(builder, (uint8_t) JOIN_CHANNEL);
    cpt_builder_serialize(builder, req_buffer);

    tcp_client_send(client_info->fd, req_buffer);
    response_msg = (uint8_t *)tcp_client_recv(client_info->fd);
    cpt_parse_response(&response, response_msg);

    cpt_builder_reset(builder);
    if (response.code == (uint8_t)SUCCESS)
    {
        client_info->channel = channel_id;
    } else { return -1; }

    return 0;
}


int cpt_create_channel(void * cpt, char * members, bool is_private)
{
    CptResponse response;
    CptBuilder * builder;
    LinkedList * channels;
    uint8_t * response_msg;
    CptClientInfo * client_info;
    uint8_t req_buffer[LG_BUFF_SIZE];

    if ( is_private ) { puts("Handle private channel"); } // !
    client_info = (CptClientInfo *)cpt;
    channels = client_info->channels;
    builder = client_info->builder;
    if ( !(client_info->fd) )
    {
        const char * message = "Not connected!";
        write(STDERR_FILENO, message, strlen(message));
        return EXIT_FAILURE;
    }

    cpt_builder_cmd(builder, (uint8_t) CREATE_CHANNEL);
    if ( members ) { cpt_builder_msg(builder, members); }
    cpt_builder_serialize(builder, req_buffer);

    tcp_client_send(client_info->fd, req_buffer);
    response_msg = (uint8_t *)tcp_client_recv(client_info->fd);
    cpt_parse_response(&response, response_msg);

    cpt_builder_reset(builder);
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
    CptBuilder * builder;
    uint8_t * response_msg;
    CptClientInfo * client_info;
    uint8_t req_buffer[LG_BUFF_SIZE];

    client_info = (CptClientInfo *)cpt;
    builder = client_info->builder;
    if ( !(client_info->fd) )
    {
        const char * message = "Not connected!";
        write(STDERR_FILENO, message, strlen(message));
        return EXIT_FAILURE;
    }

    cpt_builder_cmd(builder, (uint8_t) LEAVE_CHANNEL);
    cpt_builder_serialize(builder, req_buffer);

    tcp_client_send(client_info->fd, req_buffer);
    response_msg = (uint8_t *)tcp_client_recv(client_info->fd);
    cpt_parse_response(&response, response_msg);

    cpt_builder_reset(builder);
    delete_node(
            client_info->channels,
            (Predicate) compare_channels,
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
    client_info = malloc_safely(sizeof(struct cpt_client_info), STDERR_FILENO); // !
    if ( !(client_info->builder = cpt_builder_init()) ) { return NULL; }
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
        if ( client_info->builder ) { cpt_builder_destroy(client_info->builder);    }
        if ( client_info->ip ) { free(client_info->ip); client_info->ip = NULL;     }
        if ( client_info->ip ) { free(client_info->port); client_info->port = NULL; }
        free(client_info);
        client_info = NULL;
    }
}


void cpt_parse_response(CptResponse * response, uint8_t * buffer)
{
    parse( // !
        buffer, "C512s",
        &response->code, &response->buffer
    );
}


bool compare_channels(const uint16_t * channel_id, const uint16_t * target_channel)
{
    return *channel_id == *target_channel;
}

