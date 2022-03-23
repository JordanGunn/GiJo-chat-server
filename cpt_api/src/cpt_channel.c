//
// Created by jordan on 2022-03-18.
//

#include "cpt_channel.h"


Channel * init_global_channel()
{
    Channel * gc;
    Users * gc_users;
    User * root_user;
    UserNode * user_node;

    root_user = user_init(GC_ROOT_USER_ID, (uint16_t)GC_ROOT_USER_FD, GC_ROOT_USER_NAME);
    user_node = create_user_node(root_user);
    if (!root_user) { return NULL ;}
    gc_users = users_init(user_node);

    gc = channel_init(GC_ID, gc_users, GC_NAME, GC_ACCESS);

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


Channel * channel_init(uint16_t id, Users * users, char * name, bool is_private)
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
    channel->name = strdup(name);
    channel->is_private = is_private;

    return channel;
}


void channel_destroy(Channel * channel)
{
    if (channel)
    {
        if (channel->name)
        {
            free(channel->name);
            channel->name = NULL;
        }

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
    channel_node = (ChannelNode *)get_head_node(list);
    return channel_node;
}


void push_channel(Channels * channels, Channel * channel)
{
    LinkedList * list;
    list = (LinkedList *) channels;
    push_data(list, channel, sizeof(struct channel_struct));
}


void print_channel(Channel * channel)
{
    char buffer[LG_BUFF_SIZE] = {0};
    char * channel_access;

    channel_access = (channel->is_private) ? ACCESS_PRIVATE : ACCESS_PUBLIC;

    sprintf(buffer,
        "ID: %d\t"          \
        "NAME: %s\t"        \
        "ACCESS: %s\n",     \
        channel->id, channel->name, channel_access
    );
    printf("%s\n", buffer);
    for_each((LinkedList *)channel->users, (Consumer) print_user); // !!!!!!!!!!!!!!!!!!
}


int delete_channel(Channels * channels, int id)
{
    int result;
    Comparator find_id;

    result = SYS_CALL_FAIL;
    find_id = (Comparator) find_user_id;
    if ( channels )
    {
        result = delete_node((LinkedList *) channels, find_id, &id);
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

    while (user_iterator->next_user)
    {
        user = user_iterator->user;
        user_str = user_to_string(user);
        strncat(buffer, user_str, strlen(user_str));
        free(user_str);   // !
        user_str = NULL;
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

    free(channels); channels = NULL;
}

Channel * find_channel(Channels * dir, uint16_t id)
{
    Channel * channel;
    LinkedList * list;
    Comparator find_id;
    ChannelNode * channel_node;

    list = (LinkedList *) dir;
    find_id = (Comparator) find_channel_id;

    channel_node = (ChannelNode *) find_node(list, find_id, &id); // !
    channel = channel_node->channel;

    return ( channel ) ? channel : NULL;
}

bool find_channel_id(Channel * channel, const int * id)
{
    return (channel->id == *id);
}

