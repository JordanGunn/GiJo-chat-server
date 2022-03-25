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
    int result;
    Channel * channel;
    CptServerInfo * info;
    ChannelNode * chan_iter;

    info = (CptServerInfo *) server_info;
    if ( !info->gc )  { return BAD_CHANNEL; }

    chan_iter = get_head_channel(info->dir); // !
    if ( !chan_iter->next_chan )
        { result = delete_user(info->gc->users, info->current_id); }

    else
    {
        while (chan_iter->next_chan) /* Remove User from all channels */
        {
            channel = chan_iter->chan;
            result = delete_user(channel->users, info->current_id);
        }
    }
    return (result >= 0) ? SUCCESS : result;
}


int cpt_get_users_response(void * server_info, int chan_id)
{
    char * users_str;
    Channel * channel;
    CptServerInfo * info;

    info = (CptServerInfo *) server_info;
    if ( !info->dir )
    {
        return SERVER_ERROR;
    }

    channel = (Channel *) find_channel(info->dir, chan_id);
    users_str = channel_to_string(channel);

    if ( !users_str )
        { info->res->code = (uint8_t)CHAN_EMPTY; }
    else
        { info->res->data = (uint8_t *)users_str; }

    info->res->code = (uint8_t) SUCCESS;
    return info->res->code;
}

//TODO update cpt_msg_response input/output params
//uint8_t * cpt_msg_response(CptPacket * packet, CptResponse * res, int * result)
//{
//    CptMsgResponse * msg_res;
//    uint8_t res_buf[LG_BUFF_SIZE] = {0};
//    uint8_t res_msg_buf[LG_BUFF_SIZE] = {0};
//
//    if ( !res ) { *result = BAD_PACKET; return NULL; }
//    if ( res->data )
//    {
//        memset(res->data, 0, strlen((char *) res->data));
//    }
//
//    msg_res = cpt_msg_response_init(packet->msg, packet->channel_id, res->fd);
//    if ( !msg_res ) { *result = BAD_PACKET; }
//
//    cpt_serialize_msg(msg_res, res_msg_buf); // !
//    if ( strlen((char *)res_msg_buf) > 0 )
//    {
//        res->data = (uint8_t *)
//                strdup((char *) res_msg_buf);
//    }
//
//    cpt_serialize_response(res, res_buf);
//    cpt_response_destroy(res);
//    *result = SUCCESS;
//
//    return (uint8_t *) strdup((char *) res_buf);
//}


int cpt_create_channel_response(void * server_info, char * id_list)
{
    FilterQuery idq;
    uint16_t num_IDs;
    CptServerInfo * info;
    Channel * new_channel;
    User * user; Users * users;
    uint16_t id_buf[SM_BUFF_SIZE] = {0};

    info = (CptServerInfo *) server_info;

    if ( !info->gc )     { return BAD_CHANNEL; }
    users = NULL;
    if ( id_list )
    { /* If id_list passed, filter out the User IDs */
        id_buf[0] = info->current_id;
        num_IDs = cpt_parse_channel_query(id_list, id_buf);

        idq.num_params = num_IDs;
        idq.params = calloc(num_IDs, sizeof(uint16_t));
        memcpy(idq.params, id_buf, sizeof(uint16_t) * num_IDs);
        users = (Users *) filter( (LinkedList *)
                info->gc->users, filter_user_id, &idq, idq.num_params);
    }

    if ( !users )
    { /* if filter found requested IDs. */
        user = find_user(info->gc->users, info->current_id);
        users = users_init( create_user_node(user) );
    }

    new_channel = channel_init(
        (info->dir->length), users, "Channel", false);

    if ( new_channel )
    {
        push_channel(info->dir, new_channel);
        return SUCCESS;
    } else { return FAILURE; }
}


int cpt_join_channel_response(void * server_info, uint16_t channel_id)
{
    Channel * channel;
    CptServerInfo * info;
    User* user;

    info = (CptServerInfo *) server_info;

    channel = find_channel(info->dir, channel_id);
    if ( !channel )
    { return UKNOWN_CHANNEL; }
    else
    {
        user = find_user(info->gc->users, info->current_id);
        if ( user )
        {
            push_user(channel->users, user);
        }
        else
        {
            return FAILURE;
        }
    }
    return SUCCESS;
}


int cpt_leave_channel_response(void * server_info, uint16_t channel_id)
{
    int del_res;
    Channel * channel;
    CptServerInfo * info;

    info = (CptServerInfo *) server_info;

    if ( channel_id == 0 ) { return BAD_CHANNEL; }

    del_res = SUCCESS;
    channel = find_channel(info->dir, channel_id);
    if ( !channel )
        { return UKNOWN_CHANNEL; }
    else
        { del_res = delete_user(channel->users, info->current_id); }

    return ( del_res != SYS_CALL_FAIL ) ? SUCCESS : FAILURE;
}


size_t cpt_simple_response(CptResponse * res, uint8_t * res_buf)
{
    size_t serial_size;

    res->data = ( res->code == SUCCESS )
            ? (uint8_t *) strdup(GENERIC_SUCCESS_MSG)
            : (uint8_t *) strdup(GENERIC_FAIL_MSG);

    serial_size = cpt_serialize_response(res, res_buf);

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

    return (server_info);
}


CptServerInfo * cpt_server_info_destroy(CptServerInfo * server_info)
{

    if ( server_info )
    {
        if ( server_info->dir )
        {
            destroy_list((LinkedList *) server_info->dir);
            server_info->dir = NULL;
        }
    }

    return server_info;
}
