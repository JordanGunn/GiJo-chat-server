//
// Created by jordan on 2022-04-05.
//

#include "client_info.h"
#include "cpt_definitions.h"
#include "cpt_packet_builder.h"


ClientInfo * cpt_init_client_info(char * port, char * ip)
{
    int global_id;
    LinkedList * channels;
    ClientInfo * client_info;

    global_id = CHANNEL_ZERO;
    if ( !(client_info = malloc(sizeof(struct client_info))) )
    {
        char buf[SM_BUFF_SIZE];
        snprintf(buf, SM_BUFF_SIZE, "Failed to allocate %zu bytes...", sizeof(struct client_info));
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


void cpt_destroy_client_info(ClientInfo * client_info)
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
