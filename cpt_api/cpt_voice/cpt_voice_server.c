//
// Created by jordan on 2022-04-08.
//

#include "cpt_voice_server.h"

int cpt_create_vchannel_response(void * server_info, char * id_list)
{
    int push_res;
    uint16_t chan_id;
    ServerInfo * info;
    Channel * new_channel;
    User * user; Users * users;
    uint16_t id_buf[SM_BUFF_SIZE] = {0};

    users = NULL; push_res = 1;
    info = (ServerInfo *) server_info;

    if ( !info->gc )
    { return BAD_CHANNEL; }

    if ( id_list )
    {   /* If id_list passed, filter out the User IDs */
        id_buf[0] = (uint16_t) info->current_id;
        users = filter_channel_users(info->gc, id_buf, id_list);
    }

    if ( !users )
    {   /* if users not found, add the requesting user only. */
        user = find_user(info->gc->users, info->current_id);
        users = users_init( create_user_node(user) );
    }

    chan_id = ((uint16_t) info->dir->length) + CPT_VCHAN_MIN;
    new_channel = channel_init(chan_id, users);
    if ( new_channel )
    { push_res = push_channel(info->dir, new_channel); }

    return ( push_res ) ? SUCCESS : FAILURE;
}
