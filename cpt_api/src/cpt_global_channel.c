//
// Created by jordan on 2022-03-16.
//

#include "cpt_global_channel.h"

Channel * init_global_channel()
{
    Channel * gc;
    User * root_user;
    Users gc_users;

    root_user = user_init(GC_ROOT_USER_ID, GC_ROOT_USER_FD, GC_ROOT_USER_NAME);
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


Channels init_channel_directory(Channel * gc)
{
    Channels channel_directory;

    if ( gc )
    {
        channel_directory = channels_init(gc) ;
        return (channel_directory) ? channel_directory : NULL;
    }

    return NULL;
}


