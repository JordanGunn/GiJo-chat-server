//
// Created by jordan on 2022-03-08.
//

#include "cpt_server.h"


CptPacket * cpt_recv_packet(uint8_t * data)
{
    CptPacket * packet;
    if ( !data ) { return NULL; }

    packet = cpt_parse_packet(data);
    return ( packet ) ? packet : NULL;
}


int cpt_send_response(User * user, CptResponse * response)
{
    int result;
    uint8_t buffer[LG_BUFF_SIZE];

    if ( !user ) { return BAD_USER; }
    if ( !response ) { return BAD_RESPONSE; }

    cpt_serialize_response(response, buffer);
    result = tcp_server_send(user->fd, buffer);

    return (result >= 0) ? SUCCESS : SEND_FAILED;
}


int cpt_handle_login(Channel * gc, CptPacket * packet, int fd)
{
    char * name;
    User * user;

    if ( !gc ) { return BAD_CHANNEL; }
    if ( !packet ) { return BAD_PACKET; }
    if ( (gc->id != CHANNEL_ZERO) ) { return BAD_CHANNEL; }

    name = ( packet->msg ) ? (char *)packet->msg : DEFAULT_USER_NAME;
    if ( strlen(name) > MAX_NAME_SIZE ) { return NAME_TOO_LONG; }

    if ( (gc->users) )
    {
        user = user_init( ++(gc->users->length), fd, name );
        push_user(gc->users, user);
    }

    return SUCCESS;
}


int cpt_handle_send(Channel * channel, User * sender, CptPacket * packet)
{
    Users users;
    User * receiver;
    CptResponse * res;
    CptMsgResponse * msg_res;
    UserNode * user_iterator;
    uint8_t res_msg_buf[LG_BUFF_SIZE];

    if ( !channel )     { return BAD_CHANNEL; }
    if ( !packet )      { return BAD_PACKET;  }
    if ( !packet->msg ) { return MSG_EMPTY;   }

    msg_res = cpt_msg_response_init(packet->msg, packet->channel_id, sender->id);
    cpt_serialize_msg(msg_res, res_msg_buf); // !

    if ( !msg_res ) { return BAD_PACKET; }

    res = cpt_response_init(MSG_RESPONSE, res_msg_buf); // !

    users = channel->users;
    user_iterator = get_head_node(users); // !
    while (user_iterator->next)
    {
        receiver = user_iterator->data;
        cpt_send_response(receiver, res);
    }

    if ( res ) { cpt_response_destroy(res); }
    return SUCCESS;
}


int cpt_handle_logout(Channel * gc, Channels dir, User * user)
{
    int target_id;
    Channel * channel;
    Comparator find_id;
    ChannelNode * channel_iterator;

    if ( !gc )     { return BAD_CHANNEL; }
    if ( !user )   { return BAD_USER;    }

    target_id = user->id;
    find_id = (Comparator)find_user_id;
    channel_iterator = get_head_node(dir); // !

    while (channel_iterator->next) /* Remove User from all channels */
    {
        channel = channel_iterator->data;
        delete_node(channel->users, find_id, &target_id);
    }

    return SUCCESS;
}


int cpt_handle_get_users(Channels dir, User * user, CptPacket * packet)
{
    char * users_str;
    CptResponse * res;
    Channel * channel;

    if ( !user )     { return BAD_USER;   }
    if ( !packet )   { return BAD_PACKET; }

    channel = cpt_find_channel(dir, packet->channel_id);
    users_str = channel_to_string(channel);
    if ( !users_str ) { return CHAN_EMPTY; }

    res = cpt_response_init((uint16_t) GET_USERS, (uint8_t *) users_str);

    if ( !res ) { return BAD_PACKET; }
    if ( (cpt_send_response(user, res) < 0) ) { return -1; }

    cpt_response_destroy(res);
    return SUCCESS;
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
    else { push_user(channel->users, user);  }

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
