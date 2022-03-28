//
// Created by jordan on 2022-03-27.
//

#include "messenger.h"


Messenger * messenger_init(void * sh_mem_type)
{
    Messenger * msgr;

    msgr = malloc(sizeof(struct messenger));
    if ( !msgr ) { return NULL; }

    if ( sh_mem_type )
    {
        msgr->sh_mem_type = sh_mem_type;
    } else { sh_mem_type = NULL; }

    msgr->name   = strdup(MSG_SHM_NAME);
    msgr->consumer = NULL;
    msgr->producer = NULL;

    return msgr;
}


MsgProducer * producer_init(Messenger * msgr)
{
    MsgProducer * prod;

    prod = malloc(sizeof(struct messenger));
    if ( !prod ) { return NULL; }

    prod->fd_o = shm_open(MSG_SHM_NAME, O_CREAT | O_RDWR, 0666);
    if ( prod->fd_o != SYS_CALL_FAIL )
    {
        ftruncate(prod->fd_o, MAX_MSG_SIZE);
    }

    if ( msgr->sh_mem_type )
    {
        prod->sh_mem = msgr->sh_mem_type;
    }

    prod->sh_mem = mmap(
            0, MAX_MSG_SIZE, PROT_WRITE, MAP_SHARED, prod->fd_o, 0);

    msgr->producer = prod;
    return prod;
}


MsgConsumer * consumer_init(Messenger * msgr)
{
    MsgConsumer * cons;

    cons = malloc(sizeof(struct messenger));
    if ( !cons ) { return NULL; }

    cons->fd_i = shm_open(MSG_SHM_NAME, O_RDONLY, 0666);
    if ( cons->fd_i != SYS_CALL_FAIL )
    {
        ftruncate(cons->fd_i, MAX_MSG_SIZE);
    }

    if ( msgr->sh_mem_type )
    {
        cons->sh_mem = msgr->sh_mem_type;
    }

    msgr->consumer = cons;
    return cons;
}


void messenger_destroy(Messenger * msgr)
{
    if ( msgr )
    {
        if ( msgr->producer )
        {
            free(msgr->producer);
        }

        if ( msgr->consumer )
        {
            free(msgr->consumer);
        }

        free(msgr);
        msgr = NULL;
    }
}



