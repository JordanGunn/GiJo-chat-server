//
// Created by jordan on 2022-03-08.
//

#include "cpt_server.h"

int cpt_login_response(Channel * gc, CptPacket * packet, int id)
{
    char * name;
    User * user;

    if ( !gc ) { return BAD_CHANNEL; }
    if ( !packet ) { return BAD_PACKET; }
    if ( (gc->id != CHANNEL_ZERO) ) { return BAD_CHANNEL; }

    name = ( packet->msg ) ? (char *)packet->msg : DEFAULT_USERNAME;
    if ( strlen(name) > MAX_NAME_SIZE ) { return NAME_TOO_LONG; }

    if ( gc->users )
    {
        user = user_init(id, id, name ); // Set ID same as file descriptor.
        push_channel_user(gc, user);
    }

    return SUCCESS;
}


int cpt_logout_response(Channel * gc, Channels * dir, int id)
{
    int result;
    Channel * channel;
    ChannelNode * chan_iter;

    if ( !gc )  { return BAD_CHANNEL; }

    chan_iter = get_head_channel(dir); // !
    if ( !chan_iter->next_chan )
        { result = delete_user(gc->users, id); }

    else
    {
        while (chan_iter->next_chan) /* Remove User from all channels */
        {
            channel = chan_iter->chan;
            result = delete_user(channel->users, id);
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


uint8_t * cpt_msg_response(CptPacket * packet, CptResponse * res, int * result)
{
    CptMsgResponse * msg_res;
    uint8_t res_buf[LG_BUFF_SIZE] = {0};
    uint8_t res_msg_buf[LG_BUFF_SIZE] = {0};

    if ( !res ) { *result = BAD_PACKET; return NULL; }
    if ( res->data )
    {
        memset(res->data, 0, strlen((char *) res->data));
    }

    msg_res = cpt_msg_response_init(packet->msg, packet->channel_id, res->fd);
    if ( !msg_res ) { *result = BAD_PACKET; }

    cpt_serialize_msg(msg_res, res_msg_buf); // !
    if ( strlen((char *)res_msg_buf) > 0 )
    {
        res->data = (uint8_t *)
                strdup((char *) res_msg_buf);
    }

    cpt_serialize_response(res, res_buf);
    cpt_response_destroy(res);
    *result = SUCCESS;

    return (uint8_t *) strdup((char *) res_buf);
}


int cpt_create_channel_response(Channel * gc, Channels * dir, CptPacket * packet, int id)
{
    FilterQuery idq;
    uint16_t num_IDs;
    Channel * new_channel;
    User * user; Users * users;
    uint16_t id_buf[SM_BUFF_SIZE] = {0};

    if ( !gc )     { return BAD_CHANNEL; }
    if ( !packet ) { return BAD_PACKET;  }

    users = NULL;
    if ( packet->msg )
    {
        id_buf[0] = (uint16_t) id;
        num_IDs = cpt_parse_channel_query(packet, id_buf);// !!!

        idq.num_params = num_IDs;
        idq.params = calloc(num_IDs, sizeof(uint16_t));
        memcpy(idq.params, id_buf, sizeof(uint16_t) * num_IDs);
        users = (Users *) filter( (LinkedList *)
                gc->users, filter_user_id, &idq, idq.num_params);
    }

    if ( !users ) /* if filter found requested IDs. */
    {
        user = find_user(gc->users, id);
        users = users_init( create_user_node(user) );
    }

    new_channel = channel_init(
        (dir->length + 1), users, "Channel", false);

    if ( new_channel )
    {
        push_channel(dir, new_channel);
        return SUCCESS;
    } else { return FAILURE; }
}


int cpt_handle_join_channel(Channels * dir, User * user, CptPacket * packet)
{
    Channel * channel;

    if ( !packet ) { return BAD_PACKET; }
    if ( !user )   { return BAD_USER;   }

    channel = find_channel(dir, packet->channel_id);
    if ( !channel ) { return UKNOWN_CHANNEL; }
    push_channel_user(channel, user);

    return SUCCESS;
}


int cpt_handle_leave_channel(Channels * dir, User * user, CptPacket * packet)
{
    int result;
    Channel * channel;
    Comparator find_id;

    if ( !packet )                  { return BAD_PACKET;  }
    if ( !user )                    { return BAD_USER;    }
    if ( packet->channel_id == 0 )  { return BAD_CHANNEL; }

    channel = find_channel(dir, packet->channel_id);
    if ( !channel ) { return UKNOWN_CHANNEL; }

    result = delete_user(channel->users, packet->channel_id);

    return ( result < 0 ) ? SERVER_ERROR : SUCCESS;
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
