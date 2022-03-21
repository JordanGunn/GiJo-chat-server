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

    if ( (gc->users) )
    {
        user = user_init(id, id, name ); // Set ID same as file descriptor.
        push_user(gc->users, user);
    }

    return SUCCESS;
}


int cpt_logout_response(Channel *gc, Channels dir, int id)
{
    Channel * channel;
    Comparator find_id;
    ChannelNode * channel_iterator;

    if ( !gc )  { return BAD_CHANNEL; }

    find_id = (Comparator)find_user_id;
    channel_iterator = get_head_node(dir); // !

    while (channel_iterator->next) /* Remove User from all channels */
    {
        channel = channel_iterator->data;
        delete_node(channel->users, find_id, &id);
    }

    return SUCCESS;
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


uint8_t * cpt_get_users_response(Channels dir, CptPacket *packet, CptResponse * res)
{
    char * users_str;
    Channel * channel;
    uint8_t res_buf[LG_BUFF_SIZE] = {0};

    if ( !packet )
    {
        res->code = (uint8_t) BAD_PACKET;
        return NULL;
    }

    channel = cpt_find_channel(dir, packet->channel_id);
    users_str = channel_to_string(channel);
    if ( !users_str )
    { res->code = (uint8_t) CHAN_EMPTY; }
    else { res->data = (uint8_t *)users_str; }
    res->code = (uint8_t) SUCCESS;
    cpt_serialize_response(res, res_buf);
    cpt_response_destroy(res);

    return (uint8_t *) strdup((char *) res_buf);
}


int cpt_handle_create_channel(Channel * gc, Channels dir, User * user, CptPacket * packet)
{
    Users users;
    uint16_t * IDs;
    FilterQuery idq;
    uint16_t num_IDs;
    Comparator id_filter;
    Channel * new_channel;

    if ( !gc )                     { return BAD_CHANNEL; }
    if ( !packet )                 { return BAD_PACKET;  }
    if ( packet->channel_id == 0 ) { return BAD_CHANNEL; }

    IDs = NULL;
    users = NULL;
    if ( packet->msg )
    {
        num_IDs = cpt_parse_channel_query(packet, IDs);  // !!!
        idq.params = IDs;
        idq.num_params = num_IDs;
        id_filter = (Comparator) filter_user_id;
        users = filter(gc->users, id_filter, &idq, idq.num_params);
    }

    if (users) /* if filter found requested IDs. */
    {
        new_channel = channel_init(++gc->users->length, users, "Channel", false);
        push_user(new_channel->users, user);
        push_channel(dir, new_channel);
    }
    else /* else create a channel with the user who made the request. */
    {
        users = users_init(user);
        new_channel = channel_init(++(gc->users->length), users, "Channel", false);
        push_user(new_channel->users, user);
        push_channel(dir, new_channel);
    }

    return SUCCESS;
}


int cpt_handle_join_channel(Channels dir, User * user, CptPacket * packet)
{
    Channel * channel;

    if ( !packet ) { return BAD_PACKET; }
    if ( !user )   { return BAD_USER;   }

    channel = cpt_find_channel(dir, packet->channel_id);

    if ( !channel ) { return UKNOWN_CHANNEL; }
    push_user(channel->users, user);

    return SUCCESS;
}


int cpt_handle_leave_channel(Channels dir, User * user, CptPacket * packet)
{
    int result;
    Channel * channel;
    Comparator find_id;

    if ( !packet )                  { return BAD_PACKET;  }
    if ( !user )                    { return BAD_USER;    }
    if ( packet->channel_id == 0 )  { return BAD_CHANNEL; }

    channel = cpt_find_channel(dir, packet->channel_id);
    if ( !channel ) { return UKNOWN_CHANNEL; }

    find_id = (Comparator) find_channel_id;
    result = delete_node(channel->users, find_id, &packet->channel_id);

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
