//
// Created by jordan on 2022-03-16.
//

#ifndef CPT_CPT_GLOBAL_CHANNEL_H
#define CPT_CPT_GLOBAL_CHANNEL_H

#include "cpt_gc_config.h"
#include "cpt_channel.h"
#include "cpt_user.h"
#include "cpt_server.h"


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
Channels init_channel_directory(Channel * gc);



#endif //CPT_CPT_GLOBAL_CHANNEL_H
