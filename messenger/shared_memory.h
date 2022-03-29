//
// Created by Giwoun Bae on 2022-03-28.
//

#ifndef TEST_SMEM_SHARED_MEMORY_H
#define TEST_SMEM_SHARED_MEMORY_H


char * shmem_attach(char *filename, int size);
int shmem_detach(char *block);
int shmem_destroy(char *filename);

#define BLOCK_SIZE 4096
#define FILENAME "sharedMemory.c"


#endif //TEST_SMEM_SHARED_MEMORY_H
