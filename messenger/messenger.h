//
// Created by jordan on 2022-03-27.
//

#ifndef CPT_MESSENGER_H
#define CPT_MESSENGER_H

#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define MAX_MSG_SIZE 4096
#define MSG_SHM_NAME "cpt_messenger"


typedef struct producer MsgProducer;
struct producer
{
    int fd_o;
    void * data;
    void * sh_mem;
    FILE * stream_o;
};


typedef struct consumer MsgConsumer;
struct consumer
{
    int fd_i;
    void * data;
    void * sh_mem;
    FILE * stream_i;
};


typedef struct messenger Messenger;
struct messenger
{
    char * name;
    void * sh_mem_type;
    MsgProducer * producer;
    MsgConsumer * consumer;
};


/**
 * Initialize Messenger object.
 *
 * @return  Pointer to Messenger object.
 */
Messenger * messenger_init();


/**
 *
 * @param msgr
 * @return
 */
MsgProducer * producer_init(Messenger * msgr);


/**
 *
 * @param msgr
 * @return
 */
MsgConsumer * consumer_init(Messenger * msgr);


/**
 * Destroy Messenger object.
 *
 * @return  Pointer to Messenger object.
 */
void messenger_destroy(Messenger * msgr);


#endif //CPT_MESSENGER_H
