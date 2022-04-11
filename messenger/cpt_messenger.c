//
// Created by jordan on 2022-03-28.
//
#include "shared_memory.h"
#include "common.h"


 int main()
{
    char * block;
    int flag;
    const char * div;
    const char * title;
    block = shmem_attach(FILENAME, BLOCK_SIZE);
    div = "****************************************************************\n";
    title = "***************** C H A T    M E S S E N G E R *****************\n";

    strncat(block, div, strlen(div));
    strncat(block, title, strlen(title));
    strncat(block, div, strlen(div));
    shmem_detach(block);



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


        shmem_detach(block);
    }
    memset(block, 0, BLOCK_SIZE);
    shmem_destroy(FILENAME);
    return 0;
}

