//
// Created by Giwoun Bae on 2022-03-28.
//

#ifndef TEST_SMEM_SHARED_MEMORY_H
#define TEST_SMEM_SHARED_MEMORY_H


/**
 *
 * @param filename
 * @param size
 * @return
 */
char * shmem_attach(char *filename, int size);


/**
 *
 * @param block
 * @return
 */
int shmem_detach(char *block);


/**
 *
 * @param filename
 * @return
 */
int shmem_destroy(char *filename);

#define BLOCK_SIZE 4096
#define FILENAME "./darcy"


#endif //TEST_SMEM_SHARED_MEMORY_H
