//
// Created by jordan on 2022-03-16.
//

#ifndef CPT_GLOBALCHANNEL_H
#define CPT_GLOBALCHANNEL_H

#include "cpt_server.h"

typedef Channel * GlobalChannel;
typedef Channels ChannelDirectory;


/**
 * Initialize the GlobalChannel object.
 *
 * A Channel Object with predefined
 * properties, which are restricted
 * from regular channels.
 *
 * @return Pointer to GlobalChannel object.
 */
GlobalChannel init_global_channel();


/**
 * Initialize channel directory object.
 *
 * Creates a ChannelDirectory object, i.e.
 * A LinkedList of Channel objects.
 *
 * @param global_channel Global Channel object.
 * @return ChannelDirectory object.
 */
ChannelDirectory init_channel_directory(GlobalChannel global_channel);



#endif //CPT_GLOBALCHANNEL_H
