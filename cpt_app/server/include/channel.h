//
// Created by jordan on 2022-03-18.
//

#ifndef CPT_CHANNEL_H
#define CPT_CHANNEL_H

#include "linked_list.h"
#include "user.h"


/**
 * A Channel object.
 *
 * Contains useful data members, as
 * well as a LinkedList of User objects.
 */
typedef struct channel_struct Channel;
struct channel_struct /* may consider adding a fd to this object... */
{
    int      fd;
    uint16_t id;
    Users *  users;
};


/**
* ChannelNode object.
*
* Nodes for Channels object.
*/
typedef struct channel_node ChannelNode;
struct channel_node
{
    Channel * chan;
    ChannelNode * next_chan;
    size_t channel_size;
};


/**
 * Channels object - a LinkedList of type Channel.
 */
typedef struct channels Channels;
struct channels
{
    int length;
    ChannelNode ** channel_head;
    ChannelNode ** channel_tail;
};


/**
 * Initialize the GlobalChannel object.
 *
 * A Channel Object with predefined
 * properties, which are restricted
 * from regular channels.
 *
 * @return Pointer to GlobalChannel object.
 */
Channel * init_global_channel(void);


/**
 * Initialize channel directory object.
 *
 * Creates a ChannelDirectory object, i.e.
 * A LinkedList of Channel objects.
 *
 * @param gc Global Channel object.
 * @return ChannelDirectory object.
 */
Channels * init_channel_directory(Channel * gc);


/**
 * Initialize Channel object.
 *
 * Initialize a Channel object, allocating
 * any necessary memory and initializing data
 * members.
 *
 * @param id         The channel id.
 * @param users      A Users object (pointer to a LinkedList).
 * @return Pointer to a Channel object.
 */
Channel * channel_init(uint16_t id, Users * users);


/**
 * Destroy Channel object.
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
void channel_destroy(Channel * channel);


/**
 * Remove a ChannelNode from a LinkedList of Channel objects.
 *
 * @param channels    Pointer to a LinkedList of Channel objects.
 * @param channel_id
 * @return
 */
int channel_delete(Channels * channels, int channel_id);


/**
 * Push a user onto a Users list.
 *
 * @param users Pointer to a linked list of User objects.
 * @param user  New user to add.
 */
int push_channel(Channels * channels, Channel * channel);


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
ChannelNode * create_channel_node(Channel * channel);


/**
 * Push a User object onto the Channel->users.
 *
 * An interface for pushing users onto a channel by
 * referencing the Channel, rather than the Channel->users.
 * Behaviour is identical to push_user().
 *
 * @param channel   Pointer to a Channel object.
 * @param user      Pointer to a User object.
 */
void push_channel_user(Channel * channel, User * user);



/**
 * Get head node from a LinkedList of Channel objects.
 *
 * Gets the de-referenced head node from a Channels
 * object - a LinkedList of type Channel, and returns
 * the ChannelNode at the head of the list.
 *
 * @param channels  Pointer to a LinkedList of Channel objects.
 * @return Pointer to the ChannelNode object at the head.
 */
ChannelNode * get_head_channel(Channels * channels);


/**
 * Initialize Channels object.
 *
 * A wrapper for the LinkedList object
 * that mainly provides semantics.
 *
 * @param channel A pointer to a Channel object.
 * @return a Channels object (Pointer to a LinkedList).
 */
Channels * channels_init(ChannelNode * channel_node);



/**
 * Destroy Channels object.
 *
 * Destroy Channels object, freeing any dynamically
 * allocated memory.
 *
 * @param channels  A LinkedList of Channel objects.
 */
void channels_destroy(Channels * channels);


/**
 * Find a channel in a list of channels.
 *
 * Searches for a specific channel based on the
 * Comparator function pointer
 *
 * @param channel A pointer to a Channel object.
 * @return a Channels object (Pointer to a LinkedList).
 */
Channel * find_channel(Channels * dir, uint16_t id);


/**
 * Print details about the Channel object.
 *
 * @param channel A Channel object.
 */
char * channel_to_string(Channel * channel);


/**
 * A helper for cpt_leave_channel.
 *
 * comparator function pointer that compares
 * channel IDs in a LinkedList object.
 *
 * @param channel_id      A channel ID.
 * @param target_channel  Another channel ID.
 * @return
 */
bool compare_channels(const uint16_t * channel_id, const uint16_t * target_channel);


/**
 * Parse requested user IDs from body of
 * CREATE_CHANNEL cpt protocol message.
 *
 * @param id_list  MSG field of received CREATE_CHANNEL request.
 * @return Number of requested IDs in the id_buf.
 */
Users * filter_channel_users(Channel * channel, uint16_t * id_buf, char * id_list);


/**
 *
 * @param id_list
 * @param id_buf
 * @return
 */
uint16_t parse_channel_query(char * id_list, uint16_t * id_buf);

// =====================
// C O M P A R A T O R S
// =====================

bool find_channel_id(Channel * channel, const int * id);
bool find_channel_name(Channel * channel, int * id);
bool filter_channels_public(Channel * channel, FilterQuery * filter_query);
bool filter_channels_private(Channel * channel, FilterQuery * filter_query);


// =================
// C O N S U M E R S
// =================

/**
 * Print details about the Channel object.
 *
 * @param channel A Channel object.
 */
void print_channel(Channel * channel);


// =================
// S U P P L I E R S
// =================


#endif //CPT_CHANNEL_H
