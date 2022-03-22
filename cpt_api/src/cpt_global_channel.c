//
// Created by jordan on 2022-03-16.
//

#include "cpt_global_channel.h"

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


