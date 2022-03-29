//
// Created by Giwoun Bae on 2022-03-28.
//
#include <sys/shm.h>
#include <string.h>


#include "shared_memory.h"

#define IPC_ERROR (-1)

static int get_shared_block(char *filename, int size)
{
    key_t key;

    //  Request a key
    //  The key is linked to a filename, so that other programs can access it.
    key = ftok(filename, 0);
    if (key == IPC_ERROR)
    {
        return IPC_ERROR;
    }

    //   get shared block ---- create it if it doesn't exist.
    return shmget(key, size, 0644 | IPC_CREAT);
}


char * shmem_attach(char *filename, int size)
{
    int shared_block_id = get_shared_block(filename, size);
    char *result;

    if(shared_block_id == IPC_ERROR)
    {
        return NULL;
    }

    // map the shared block into this process's memory
    // and give me a pointer to it
    result = shmat(shared_block_id, NULL, 0);
    if (result == (char *) IPC_ERROR)
    {
        return NULL;
    }

    return result;
}



int shmem_detach(char * block)
{
    return (shmdt(block) != IPC_ERROR);
}



int shmem_destroy(char * name)
{
    int block_id = get_shared_block(name, 0);

    if (block_id == IPC_ERROR) { return -1; }

    return ( shmctl(block_id, IPC_RMID, NULL) != IPC_ERROR );
}
