//
// Created by jordan on 2022-03-17.
//

#define SA_CHECK_NULL 1
#define SA_CHECK_NEG1 2
#include "common.h"

void * malloc_safely(size_t target_size, int stream_fd)
{
    void * target;
    char buffer[SM_BUFF_SIZE];
    if ( !(target = malloc(target_size)) )
    {
        snprintf(buffer, SM_BUFF_SIZE, "Failed to allocate memory of size %zu bytes...", target_size);
        write(stream_fd, buffer, strlen(buffer));
        return NULL;
    }

    return target;
}

