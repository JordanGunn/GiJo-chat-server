//
// Created by jordan on 2022-03-08.
//

#include "cpt_server.h"

int cpt_login_response(void * server_info, char * name)
{
    int id;
    User * user;
    CptServerInfo * info;

    info = (CptServerInfo *) server_info;

    id = info->current_id;
    if ( !info->gc ) { return BAD_CHANNEL; }
    if ( (info->gc->id != CHANNEL_ZERO) ) { return BAD_CHANNEL; }

    name = ( name ) ? name : DEFAULT_USERNAME;
    if ( strlen(name) > MAX_NAME_SIZE ) { return NAME_TOO_LONG; }

    if ( info->gc->users )
    {
        user = user_init(id, id, name ); // Set ID same as file descriptor.
        push_channel_user(info->gc, user);
    }

    return SUCCESS;
}


int cpt_logout_response(void * server_info)
{
    int lo_res;
    Channel * chan;
    CptServerInfo * info;
    ChannelNode * chan_iter;

    info = (CptServerInfo *) server_info;
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
            chan = chan_iter->chan;
            if (chan->id != CHANNEL_ZERO )
            {
                user_delete(chan->users, info->current_id);
                if (chan->users->length == 0 )
                    { channel_delete(info->dir, chan->id); }
            }
            chan_iter = chan_iter->next_chan;
        }
        lo_res = user_delete(info->gc->users, info->current_id);
    }
    return (lo_res) ? SUCCESS : FAILURE;
}


int cpt_get_users_response(void * server_info, uint16_t channel_id)
{

    char * res_str;
    Channel * channel;
    CptServerInfo * info;

    info = (CptServerInfo *) server_info;

    channel = find_channel(info->dir, channel_id);
    if ( channel )
    {
        res_str = channel_to_string(channel);
        if ( res_str )
            { info->res->code = SUCCESS; }
        else
            { info->res->code = CHAN_EMPTY; }
    }
    else { info->res->code = BAD_CHANNEL; }

    if ( info->res->code != SUCCESS )
    {
        res_str = strdup("Failed to get users...");
    }

    info->res->data = (uint8_t *) res_str;
    info->res->data_size = strlen(res_str);

    return info->res->code;
}


int cpt_create_channel_response(void * server_info, char * id_list)
{
    int push_res;
    CptServerInfo * info;
    Channel * new_channel;
    User * user; Users * users;
    uint16_t id_buf[SM_BUFF_SIZE] = {0};

    users = NULL; push_res = 1;
    info = (CptServerInfo *) server_info;

    if ( !info->gc )
        { return BAD_CHANNEL; }

    if ( id_list )
    {   /* If id_list passed, filter out the User IDs */
        id_buf[0] = info->current_id;
        users = filter_channel_users(info->gc, id_buf, id_list);
    }

    if ( !users )
    {   /* if users not found, add the requesting user only. */
        user = find_user(info->gc->users, info->current_id);
        users = users_init( create_user_node(user) );
    }

    new_channel = channel_init((info->dir->length), users);
    if ( new_channel )
        { push_res = push_channel(info->dir, new_channel); }

    return ( push_res ) ? SUCCESS : FAILURE;
}


int cpt_join_channel_response(void * server_info, uint16_t channel_id)
{
    int result;
    User * user;
    Channel * channel;
    CptServerInfo * info;

    result = false;
    info = (CptServerInfo *) server_info;

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
    CptServerInfo * info;

    info = (CptServerInfo *) server_info;

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


//int cpt_send_response(void * server_info, uint16_t channel_id)
//{
//    int result;
//    Users * users;
//    Channel * channel;
//    CptServerInfo * info;
//    User * user, next_user;
//
//    result = false;
//    info = (CptServerInfo *) server_info;
//
//
//    channel = find_channel(info->dir, info->channel_id);
//    if ( channel )
//    {
//        user = get_head_user()
//
//        if ( user ) /* User is already part of channel */
//        { result = true; }
//        else
//        { /* Find user in gc and push them onto requested channel */
//            user = find_user(info->gc->users, info->current_id);
//            if ( user )
//            { result = push_user(channel->users, user); }
//        }
//    }
//    if ( channel_id == 0 ) { return BAD_CHANNEL; }
//
//    return SUCCESS;
//}



size_t cpt_simple_response(CptResponse * res, uint8_t * res_buf)
{
    size_t serial_size;

    res->data = ( res->code == SUCCESS )
            ? (uint8_t *) strdup(GENERIC_SUCCESS_MSG)
            : (uint8_t *) strdup(GENERIC_FAIL_MSG);

    serial_size = cpt_serialize_response(res, res_buf);
    if ( res->data ) { free(res->data); res->data = NULL; }

    return serial_size;
}


CptServerInfo * cpt_server_info_init(Channel * gc, Channels * dir)
{
    CptServerInfo * server_info;

    if ( !(server_info = malloc(sizeof(struct cpt_server_info))) )
    {
        return NULL;
    }

    server_info->gc = gc;
    server_info->dir = dir;
    server_info->current_id = gc->id;
    server_info->channel_id = 0;

    return (server_info);
}


CptServerInfo * cpt_server_info_destroy(CptServerInfo * server_info)
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
