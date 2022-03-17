//
// Created by jordan on 2022-03-08.
//

#ifndef GIJO_CPT_SERVER_H
#define GIJO_CPT_SERVER_H

#include "../include/cpt_server.h"

User * init_user(int id, int fd, char * name)
{
    User * user;

    if ( !(name) ) { name = DEFAULT_USER_NAME; }
    else if (strlen(name) > MAX_NAME_SIZE)
    {
        write(STDERR_FILENO, "Name exceeds max of %d characters...\n", MAX_NAME_SIZE);
        return NULL;
    }

    if ( !(user = malloc(sizeof(User))) )
    {
        printf("Failed to allocate memory of size %lud\n", sizeof(User));
        return NULL;
    }

    memset(user, 0, sizeof(User));
    user->fd = fd;
    user->id = id;
    user->name = strdup(name);

    return user;
}


void destroy_user(User * user)
{
    if (user)
    {
        if (user->name)
        {
            free(user->name);
            user->name = NULL;
        }
        free(user); user = NULL;
    }
}


UserNode * user_node(User * user)
{
    UserNode * user_node;

    user_node = create_node(user, sizeof(struct user_struct));
    return (user_node) ? user_node : NULL;
}


void push_user(Users users, User * user)
{
    UserNode * new_user;
    UserNode * next_user;

    new_user = create_node(user, sizeof(struct user_struct));
    next_user = *(users->head);
    (*users->head) = new_user;
    (*users->head)->next = next_user;

    if ( users->length == 1 )
    {
        (*users->tail) = next_user;   // set the first tail
    }

    users->length++;
}


void user_to_string(User * user)
{
    char buffer[MD_BUFF_SIZE] = {0};

    sprintf(buffer,
        "ID: %d\t"    \
        "NAME: %s\n", \
        user->id, user->name
    );

    printf("%s\n", buffer);
}


Users init_users(User * user)
{
    Users users;

    if (!user) { return NULL; }

    users = init_list(user, sizeof(struct user_struct));
    return (users) ? users : NULL;
}


Channel * init_channel(uint16_t id, Users users, char * name, bool is_private)
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


void destroy_channel(Channel * channel)
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


/**
 * Print details about the Channel object.
 *
 * @param channel A Channel object.
 */
void channel_to_string(Channel * channel)
{
    char buffer[MD_BUFF_SIZE] = {0};
    char * channel_access;

    channel_access = (channel->is_private) ? ACCESS_PRIVATE : ACCESS_PUBLIC;

    sprintf(buffer,
        "ID: %d\t"          \
        "NAME: %s\t"        \
        "ACCESS: %s\n",     \
        channel->id, channel->name, channel_access
    );
    printf("%s\n", buffer);
    for_each(channel->users, (Consumer) user_to_string); // !!!!!!!!!!!!!!!!!!
}


Channels init_channels(Channel * channel)
{
    Channels channels;

    if (!channel) { return NULL; }

    channels = init_list(channel, sizeof(struct channel_struct));
    return (channels) ? channels : NULL;
}


void destroy_channels(Channels channels)
{
    if (channels)
    {
        destroy_list(channels);
    }

    free(channels); channels = NULL;
}


// ===================
// P R E D I C A T E S
// ===================
//bool find_id_equal(void * data, void * param)
//{
//    test_data * td;
//    td = (test_data *)data;
//
//    return td->id == *( (int *)param );
//}

bool find_user_id(User * user, const int * id)
{
    return user->id == *id;
}


bool find_user_name(User * user, char * name)
{
    return !strcmp(user->name, name);
}


bool filter_user_id(User * user, FilterQuery * filter_query)
{
    int i, * num_crawler;
    int * IDs, num_IDs;

    IDs = (int *)filter_query->params;
    num_IDs = filter_query->num_params;

    num_crawler = IDs;
    for (i = 0; i < num_IDs; i++)
    {
        if ( user->id == *(num_crawler) ) { return true; }
        num_crawler++;
    }
    return false;
}

#endif //GIJO_CPT_SERVER_H
