//
// Created by jordan on 2022-03-16.
//

#include "../include/GlobalChannel.h"
#include "../include/global_config.h"


GlobalChannel init_global_channel()
{
    GlobalChannel gc;
    User * root_user;
    Users gc_users;

    root_user = init_user(GC_ROOT_USER_ID, GC_ROOT_USER_FD, GC_ROOT_USER_NAME);
    if (!root_user) { return NULL ;}
    gc_users = init_list(root_user, sizeof(struct user_struct));

    if ( !(gc = malloc(sizeof(struct channel_struct))) )
    {
        printf("Failed to allocate memory of size %lud\n", sizeof(Channel));
        return NULL;
    }

    memset(gc, 0, sizeof(struct channel_struct));
    gc->id = GC_ID;
    gc->name = strdup(GC_NAME);
    gc->is_private = GC_ACCESS;
    gc->users = gc_users;

    return gc;
}


ChannelDirectory init_channel_directory(GlobalChannel gc)
{
    ChannelDirectory channel_directory;

    if ( gc )
    {
        channel_directory = init_channels(gc) ;
        return (channel_directory) ? channel_directory : NULL;
    }

    return NULL;
}


