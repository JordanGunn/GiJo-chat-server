//
// Created by jordan on 2022-03-18.
//

#include "channel.h"


Channel * init_global_channel()
{
    Channel * gc;
    Users * gc_users;
    User * root_user;
    UserNode * user_node;

    root_user = user_init(GC_ROOT_USR_ID, (uint16_t) GC_ROOT_USR_FD, GC_ROOT_USR_NAME);
    user_node = create_user_node(root_user);
    if ( !root_user ) { return NULL ;}

    gc_users = users_init(user_node);
    gc = channel_init(GC_ID, gc_users);

    return gc;
}


Channels * init_channel_directory(Channel * gc)
{
    Channels * channel_directory;
    ChannelNode * channel_node;

    if ( gc )
    {
        channel_node = create_channel_node(gc);
        channel_directory = channels_init(channel_node);
        return (channel_directory) ? channel_directory : NULL;
    }

    return NULL;
}


Channel * channel_init(uint16_t id, Users * users)
{
    Channel * channel;

    if ( !(channel = malloc(sizeof(struct channel_struct))) )
    {
        printf("Failed to allocate memory of size %lud\n", sizeof(User));
        return NULL;
    }

    memset(channel, 0, sizeof(struct channel_struct));
    channel->id = id;
    channel->users = users;

    return channel;
}


void channel_destroy(Channel * channel)
{
    if (channel)
    {
        if (channel->users)
        {
            destroy_list((LinkedList *) channel->users);
        }

        free(channel); channel = NULL;
    }
}


ChannelNode * create_channel_node(Channel * channel)
{
    ChannelNode * channel_node;

    channel_node = (ChannelNode *) create_node(channel, sizeof(struct channel_struct));
    return (channel_node) ? channel_node : NULL;
}


ChannelNode * get_head_channel(Channels * channels)
{
    LinkedList * list;
    ChannelNode * channel_node;

    list = (LinkedList *) channels;
    channel_node = (ChannelNode *) get_head_node(list);
    return channel_node;
}


int push_channel(Channels * channels, Channel * channel)
{
    int push_res;
    LinkedList * list;
    list = (LinkedList *) channels;
    push_res = push_data(list, channel, sizeof(struct channel_struct));

    return ( push_res ) ? 1 : 0;
}

Users * filter_channel_users(Channel * channel, uint16_t * id_buf, char * id_list)
{
    LinkedList * list;
    FilterQuery idq;
    Users * users;

    idq.num_params = parse_channel_query(id_list, id_buf);
    idq.params = calloc(idq.num_params, sizeof(uint16_t));
    memcpy(idq.params, id_buf, sizeof(uint16_t) * idq.num_params);

    users = NULL;
    if ( channel )
    {
        list = (LinkedList *) channel->users;
        users = (Users *) filter(list, filter_user_id, &idq, idq.num_params);
    }

    if (idq.params)
        { free(idq.params); idq.params = NULL; }

    return ( users ) ? users : NULL;
}


uint16_t parse_channel_query(char * id_list, uint16_t * id_buf)
{
    char * id_str;
    int id_count;
    uint16_t id;

    id_count = 0;
    while ( strlen(id_list) )
    {
        id = strtol((char *)id_list, &id_str, 10);
        id_list = id_str;
        if ( id != 0 )
        {
            if ( id != id_buf[0] )
            {
                id_buf[id_count] = id;
            }
            id_count++;
        }
    }

    return id_count;
}


void print_channel(Channel * channel)
{
    char buffer[LG_BUFF_SIZE] = {0};

    sprintf(buffer, "CHANNEL ID: %d\t", channel->id);
    printf("%s\n", buffer);

    for_each((LinkedList *) channel->users, (Consumer) print_user); // !
}


int channel_delete(Channels * channels, int channel_id)
{
    int result;
    Comparator find_id;

    result = SYS_CALL_FAIL;
    find_id = (Comparator) find_channel_id;
    if ( channels )
    {
        result = delete_node((LinkedList *) channels, find_id, &channel_id);
    }

    return result;
}


char * channel_to_string(Channel * channel)
{
    User * user;
    char * user_str;
    UserNode * user_iterator;
    char buffer[LG_BUFF_SIZE] = {0};

    user_iterator = (UserNode *) get_head_node((LinkedList *) channel->users);

    if ( !(user_iterator->next_user) )
    { /* If only User in Channel */
        user = user_iterator->user;
        user_str = user_to_string(user);
        return strdup(user_str);
    }

    while ( user_iterator )
    { /* If multiple users in channel */
        user = user_iterator->user;
        if ( user )
        {
            user_str = user_to_string(user);
            strncat(buffer, user_str, strlen(user_str));
            free(user_str);   // !
            user_str = NULL;
        }
        user_iterator = user_iterator->next_user;
    }

    return ( strlen(buffer) > 0 ) ? strdup(buffer) : NULL;
}


Channels * channels_init(ChannelNode * channel_node)
{
    Channels * channels;

    if (!channel_node) { return NULL; }

    channels = (Channels *) init_list_node((Node *) channel_node);
    return (channels) ? channels : NULL;
}


void push_channel_user(Channel * channel, User * user)
{
    UserNode * user_node;
    user_node = create_user_node(user);
    push_node( (LinkedList *)channel->users, (Node *) user_node);
}


void channels_destroy(Channels * channels)
{
    if (channels)
    {
        destroy_list((LinkedList *) channels);
    }
}


Channel * find_channel(Channels * dir, uint16_t id)
{
    Channel * channel;
    LinkedList * list;
    Comparator find_id;
    ChannelNode * channel_node;

    list = (LinkedList *) dir;
    find_id = (Comparator) find_channel_id;

    channel_node = NULL;
    channel_node = (ChannelNode *) find_node(list, find_id, &id); // !

    if ( channel_node )
        { channel = channel_node->chan; }
    else
        { channel = NULL; }

    return channel;
}

bool find_channel_id(Channel * channel, const int * id)
{
    uint16_t target_id;

    target_id = (uint16_t) (*id);
    return (channel->id == target_id);
}

