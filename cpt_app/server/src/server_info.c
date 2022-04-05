//
// Created by jordan on 2022-04-05.
//

#include "server_info.h"


ServerInfo * cpt_server_info_init(Channel * gc, Channels * dir)
{
    ServerInfo * server_info;

    if ( !(server_info = malloc(sizeof(struct server_info))) )
    {
        return NULL;
    }

    server_info->gc = gc;
    server_info->dir = dir;
    server_info->current_id = gc->id;
    server_info->channel_id = 0;

    return (server_info);
}


ServerInfo * cpt_server_info_destroy(ServerInfo * server_info)
{

    if ( server_info )
    {
        if ( server_info->dir )
        {
            channels_destroy(server_info->dir);
            server_info->dir = NULL;
        }

        if ( server_info->gc )
        {
            if ( server_info->gc->users )
            {
                users_destroy(server_info->gc->users);
            }

            channel_destroy(server_info->gc);
        }
    }

    return server_info;
}
