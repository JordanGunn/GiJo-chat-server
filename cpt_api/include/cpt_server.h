//
// Created by jordan on 2022-03-08.
//

#ifndef CPT_CPT_SERVER_H
#define CPT_CPT_SERVER_H

#include "linked_list.h"

#define MAX_NAME_SIZE 12
#define DEFAULT_NAME "anonymous"

typedef struct user_struct User;
typedef struct channel_struct Channel;

typedef Node UserNode;
typedef Node ChannelNode;
typedef LinkedList * Channels;
typedef LinkedList * Users;


struct user_struct
{
    uint8_t id;
    uint8_t fd;
    char *  name;
};

struct channel_struct /* may consider adding a fd to this object... */
{
    uint16_t id;
    Users    users;
    bool     is_private;
    char *   name;

};

struct channel_directory
{
    uint16_t channel_count;
    Channels channels;
};


/**
 * Initialize a User object.
 *
 * Creates a User object, intialiazing any
 * data members and allocating the necessary
 * memory.
 *
 * @param id    User id.
 * @param fd    File descriptor for the user socket.
 * @param name  User name.
 * @return Pointer to a User object.
 */
User * init_user(int id, int fd, char * name)
{
    User * user;

    if ( !(name) ) { name = DEFAULT_NAME; }
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


/**
 * Destroy User object.
 *
 * Destroys a User object, freeing any memory
 * and setting necessary values to NULL.
 *
 * @param user A pointer to a User object.
 */
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


/**
 * Create a UserNode object.
 *
 * Creates a UserNode object which can be added
 * to a LinkedList. This function mostly provides
 * a wrapper for the Node object, with additional
 * semantic meaning.
 *
 * @param user A pointer to a User object.
 * @return     A pointer to a UserNode object.
 */
UserNode * user_node(User * user)
{
    UserNode * user_node;

    user_node = create_node(user, sizeof(struct user_struct));
    return (user_node) ? user_node : NULL;
}


/**
 * Initialize Users object.
 *
 * Initialize a Users object, allocating any
 * necessary memory and initializing data members.
 *
 * @param user      A pointer to a User object.
 * @return Pointer to Users LinkedList object.
 */
Users init_users(User * user)
{
    Users users;

    if (!user) { return NULL; }

    users = init_list(user, sizeof(struct user_struct));
    return (users) ? users : NULL;
}


/**
 * Initialize Channel object.
 *
 * Initialize a Channel object, allocating
 * any necessary memory and initializing data
 * members.
 *
 * @param id         The channel id.
 * @param users      A Users object (pointer to a LinkedList).
 * @param name       Name of the channel.
 * @param is_private Privacy setting of channel.
 * @return Pointer to a Channel object.
 */
Channel * init_channel(uint16_t id, Users users, char * name, bool is_private)
{

    Channel * channel;

    if (strlen(name) > MAX_NAME_SIZE)
    {
        write(STDERR_FILENO, "Name exceeds max of %d characters...\n", MAX_NAME_SIZE);
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


/**
 * Initialize Channels object.
 *
 * A wrapper for the LinkedList object
 * that mainly provides semantics.
 *
 * @param channel A pointer to a Channel object.
 * @return a Channels object (Pointer to a LinkedList).
 */
Channels init_channels(Channel * channel)
{
    Channels channels;

    if (!channel) { return NULL; }

    channels = init_list(channel, sizeof(struct channel_struct));
    return (channels) ? channels : NULL;
}

#endif //CPT_CPT_SERVER_H
