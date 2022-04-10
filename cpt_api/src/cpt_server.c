//
// Created by jordan on 2022-03-08.
//

#include "cpt_server.h"

int cpt_login_response(void * server_info, char * name)
{
    ServerInfo * info;

    info = (ServerInfo *) server_info;

    if ( !info->gc ) { return BAD_CHANNEL; }
    if ( (info->gc->id != CHANNEL_ZERO) ) { return BAD_CHANNEL; }

    name = ( name ) ? name : DEFAULT_USERNAME;
    if ( strlen(name) > MAX_NAME_SIZE ) { return NAME_TOO_LONG; }

    return SUCCESS;
}


int cpt_logout_response(void * server_info)
{
    int lo_res;
    Channel * chan;
    ServerInfo * info;
    ChannelNode * chan_iter, * next_chan;

    info = (ServerInfo *) server_info;
    if ( !info->gc )  { return BAD_CHANNEL; }

    chan_iter = get_head_channel(info->dir); // !
    if ( !chan_iter->next_chan )
    {
        lo_res = user_delete(info->gc->users, info->current_id);
    }
    else
    {
        lo_res = false;
        while ( chan_iter ) /* Remove User from all channels */
        {
            next_chan = chan_iter->next_chan;
            if (chan_iter->chan->id != CHANNEL_ZERO )
            {
                user_delete(chan_iter->chan->users, info->current_id);
                if (chan_iter->chan->users->length == 0 )
                {
                    channel_delete(info->dir, chan_iter->chan->id);

                }
            }
            chan_iter = next_chan;
        }
        lo_res = user_delete(info->gc->users, info->current_id);
    }
    return (lo_res) ? SUCCESS : FAILURE;
}


int cpt_get_users_response(void * server_info, uint16_t channel_id)
{

    int result;
    char * res_str;
    Channel * channel;
    ServerInfo * info;

    info = (ServerInfo *) server_info;

    channel = find_channel(info->dir, channel_id);
    if ( channel )
    {
        res_str = channel_to_string(channel);
        if ( res_str )
            { result = SUCCESS; }
        else
            { result = CHAN_EMPTY; }
    }
    else { result = BAD_CHANNEL; }

    if ( result != SUCCESS )
    {
        res_str = strdup("Failed to get users...");
        info->res->code = FAILURE;
    }
    else
    {
        info->res->code = (uint8_t) GET_USERS;
    }


    info->res->data = (uint8_t *) res_str;
    info->res->data_size = (uint16_t) strlen(res_str);

    return result;
}


int cpt_create_channel_response(void * server_info, char * id_list)
{
    int push_res;
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

    new_channel = channel_init(((uint16_t) info->dir->length), users);
    if ( new_channel )
        { push_res = push_channel(info->dir, new_channel); }

    return ( push_res ) ? SUCCESS : FAILURE;
}


int cpt_join_channel_response(void * server_info, uint16_t channel_id)
{
    int result;
    User * user;
    Channel * channel;
    ServerInfo * info;

    result = false;
    info = (ServerInfo *) server_info;

    channel = find_channel(info->dir, channel_id);
    if ( channel )
    {
        user = find_user(channel->users, info->current_id);
        if ( user ) /* User is already part of channel */
            { result = true; }
        else
        { /* Find user in gc and push them onto requested channel */
            user = find_user(info->gc->users, info->current_id);
            if ( user )
                { result = push_user(channel->users, user); }
        }
    }
    return ( result ) ? SUCCESS : FAILURE;
}


int cpt_leave_channel_response(void * server_info, uint16_t channel_id)
{
    int del_res;
    Channel * channel;
    ServerInfo * info;

    info = (ServerInfo *) server_info;

    del_res = false;
    if ( channel_id == CHANNEL_ZERO ) { return BAD_CHANNEL; }

    channel = find_channel(info->dir, channel_id);
    if ( channel )
    {
        del_res = user_delete(channel->users, info->current_id);
        if ( del_res )
        {
            if ( channel->users->length == 0 )
            { /* If a channel becomes empty, it ceases to exist */
                channel_delete(info->dir, channel_id);
            }
        }
    }
    return ( del_res ) ? SUCCESS : FAILURE;
}
