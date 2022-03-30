//
// Created by jordan on 2022-03-28.
//
#include "shared_memory.h"
#include "common.h"


 int main()
{
    int flag;
    char * block;

    flag = 0;
    while ( !flag )
    {
        if ( (block = shmem_attach(FILENAME, BLOCK_SIZE)) )
        {
            if ( strlen(block) )
            {
                printf("%s", block);
                memset(block, 0, strlen(block));
            }
        } else { flag = 1; }

        memset(block, 0, BLOCK_SIZE);
        shmem_detach(block);
    }

    shmem_destroy(FILENAME);
    return 0;
}

