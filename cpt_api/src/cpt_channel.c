//
// Created by jordan on 2022-03-18.
//

#include "cpt_channel.h"

Channel * channel_init(uint16_t id, Users users, char * name, bool is_private)
{
    Channel * channel;

    if (id == CHANNEL_ZERO)
    {
        write(STDERR_FILENO, "Channel 0 (zero) creation forbidden\n", SM_BUFF_SIZE);
        return NULL;
    }

    if (strlen(name) > MAX_NAME_SIZE)
    {
        write(STDERR_FILENO, "Name exceeds max name length...\n", SM_BUFF_SIZE);
        return NULL;
    }

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
            destroy_list(channel->users);
        }

        free(channel); channel = NULL;
    }
}


void push_channel(Channels channels, Channel * channel)
{
    UserNode * new_user;
    UserNode * next_user;

    new_user = create_node(channel, sizeof(struct user_struct));
    next_user = *(channels->head);
    (*channels->head) = new_user;
    (*channels->head)->next = next_user;

    if ( channels->length == 1 )
    {
        (*channels->tail) = next_user;   // set the first tail
    }

    channels->length++;
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
    for_each(channel->users, (Consumer) print_user); // !!!!!!!!!!!!!!!!!!
}


/**
 * Print details about the Channel object.
 *
 * @param channel A Channel object.
 */
char * channel_to_string(Channel * channel)
{
    User * user;
    char * user_str;
    UserNode * user_iterator;
    char buffer[LG_BUFF_SIZE] = {0};

    user_iterator = get_head_node(channel->users);

    while (user_iterator->next)
    {
        user = user_iterator->data;
        user_str = user_to_string(user);
        strncat(buffer, user_str, strlen(user_str));
        free(user_str);   // !
        user_str = NULL;
    }

    return ( strlen(buffer) > 0 ) ? strdup(buffer) : NULL;
}


Channels channels_init(Channel * channel)
{
    Channels channels;

    if (!channel) { return NULL; }

    channels = init_list(channel, sizeof(struct channel_struct));
    return (channels) ? channels : NULL;
}


void channels_destroy(Channels channels)
{
    if (channels)
    {
        destroy_list(channels);
    }

    free(channels); channels = NULL;
}

Channel * cpt_find_channel(Channels dir, uint16_t id)
{
    Channel * channel;
    LinkedList * list;
    Comparator find_id;

    list = (LinkedList *) dir;
    find_id = (Comparator) find_channel_id;

    channel = (Channel *) find_node(list, find_id, &id); // !

    return ( channel ) ? channel : NULL;
}

bool find_channel_id(Channel * channel, const int * id)
{
    return (channel->id == *id);
}

