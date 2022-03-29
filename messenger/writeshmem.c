#include <stdio.h>
#include <string.h>
#include "shared_memory.h"

/*
 * tfok
 * shmget
 * shmat
 * shmdt
 * shmctl
 */

int main (int argc, char *argvp[])
{
    char *block = shmem_attach(FILENAME, BLOCK_SIZE);
    if (!block)
    {
        return -1;
    }

    char *test = "testing";
    printf("Writing: \"%s\"\n", test);
    strncpy(block, test, BLOCK_SIZE);

    shmem_detach(block);
    return 0;
}