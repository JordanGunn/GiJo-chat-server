//
// Created by jordan on 2022-03-08.
//

#ifndef CPT_CPT_SERVER_H
#define CPT_CPT_SERVER_H

#include "linked_list.h"

#define CHANNEL_ZERO 0
#define MAX_NAME_SIZE 12
#define DEFAULT_NAME "anonymous"
#define ACCESS_PRIVATE "PRIVATE"
#define ACCESS_PUBLIC "PUBLIC"

typedef struct user_struct User;
typedef struct channel_struct Channel;
typedef Node UserNode;
typedef Node ChannelNode;
typedef LinkedList * Users;
typedef LinkedList * Channels;


/**
 * A user Object.
 *
 * Contains data members useful for
 * handling user in the CPT environment.
 */
struct user_struct
{
    uint8_t id;
    uint8_t fd;
    char *  name;
};


/**
 * A Channel object.
 *
 * Contains useful data members, as
 * well as a LinkedList of User objects.
 */
struct channel_struct /* may consider adding a fd to this object... */
{
    uint16_t id;
    Users    users;
    bool     is_private;
    char *   name;
};


/**
 * LinkedList of Channel objects.
 */
struct channel_directory
{
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
User * init_user(int id, int fd, char * name);


/**
 * Destroy User object.
 *
 * Destroys a User object, freeing any memory
 * and setting necessary values to NULL.
 *
 * @param user A pointer to a User object.
 */
void destroy_user(User * user);


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
UserNode * user_node(User * user);


/**
 * Initialize Users object.
 *
 * Initialize a Users object, allocating any
 * necessary memory and initializing data members.
 *
 * @param user      A pointer to a User object.
 * @return Pointer to Users LinkedList object.
 */
Users init_users(User * user);


/**
 * Push a user onto a Users list.
 *
 * @param users Pointer to a linked list of User objects.
 * @param user  New user to add.
 */
void push_user(Users users, User * user);


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
Channel * init_channel(uint16_t id, Users users, char * name, bool is_private);


/**
 * Push a user onto a Users list.
 *
 * @param users Pointer to a linked list of User objects.
 * @param user  New user to add.
 */
void push_channel(Channels channels, Channel * channel);


/**
 * Create a ChannelNode object.
 *
 * Creates a ChannelNode object which can be added
 * to a LinkedList. This function mostly provides
 * a wrapper for the Node object, with additional
 * semantic meaning.
 *
 * @param user A pointer to a Channel object.
 * @return     A pointer to a ChannelNode object.
 */
ChannelNode * channel_node(Channel * channel);


/**
 * Initialize Channels object.
 *
 * A wrapper for the LinkedList object
 * that mainly provides semantics.
 *
 * @param channel A pointer to a Channel object.
 * @return a Channels object (Pointer to a LinkedList).
 */
Channels init_channels(Channel * channel);


// ===================
// P R E D I C A T E S
// ===================

bool find_user_id(User * user, const int * id);
bool find_user_name(User * user, char * name);
bool filter_user_id(User * user, FilterQuery * filter_query);
bool filter_user_name(User * user, FilterQuery * filter_query);

bool find_channel_id(Channel * channel, int * id);
bool find_channel_name(Channel * channel, int * id);
bool filter_channels_public(Channel * channel, FilterQuery * filter_query);
bool filter_channels_private(Channel * channel, FilterQuery * filter_query);

// =================
// C O N S U M E R S
// =================
/**
 * Print Details about the channel.
 *
 * @param channel The target channel.
 */
void user_to_string(User * user);


/**
 * Print details about the Channel object.
 *
 * @param channel A Channel object.
 */
void channel_to_string(Channel * channel);


// =================
// S U P P L I E R S
// =================


#endif //CPT_CPT_SERVER_H
