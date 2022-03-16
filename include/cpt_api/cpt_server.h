//
// Created by jordan on 2022-03-08.
//

#ifndef GIJO_CPT_SERVER_H
#define GIJO_CPT_SERVER_H

#define MAX_NAME_SIZE 12

typedef struct user
{
    uint8_t user_id;
    uint8_t user_fd;
    char name[MAX_NAME_SIZE];
} User;


typedef struct user_node
{
    User * user;
    User * next_user;
} UserNode;


struct channel
{
    uint16_t channel_id;
    uint16_t user_count;
    bool     private;
    user *   users[];
};


struct channel_directory
{
    uint16_t channel_count;
    channel *   channels[];
};

#endif //GIJO_CPT_SERVER_H
