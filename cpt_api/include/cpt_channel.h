//
// Created by jordan on 2022-03-18.
//

#ifndef CPT_CPT_CHANNEL_H
#define CPT_CPT_CHANNEL_H

#include "cpt_definitions.h"
#include "linked_list.h"
#include "cpt_types.h"
#include "cpt_user.h"

typedef struct channels Channels;
typedef struct channel_node ChannelNode;
typedef struct channel_struct Channel;

/**
 * A Channel object.
 *
 * Contains useful data members, as
 * well as a LinkedList of User objects.
 */
struct channel_struct /* may consider adding a fd to this object... */
{
    int      fd;
    uint16_t id;
    char *   name;
    Users *  users;
    bool     is_private;
};


int delete_channel(Channels * channels, int id);


struct channels
{
    int length;
    ChannelNode ** channel_head;
    ChannelNode ** channel_tail;
};


struct channel_node
{
    Channel * chan;
    ChannelNode * next_chan;
    size_t channel_size;
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
Channel * init_global_channel();


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
 * @param name       Name of the channel.
 * @param is_private Privacy setting of channel.
 * @return Pointer to a Channel object.
 */
Channel * channel_init(uint16_t id, Users * users, char * name, bool is_private);


/**
 * Push a user onto a Users list.
 *
 * @param users Pointer to a linked list of User objects.
 * @param user  New user to add.
 */
void push_channel(Channels * channels, Channel * channel);


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


void push_channel_user(Channel * channel, User * user);


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


#endif //CPT_CPT_CHANNEL_H
