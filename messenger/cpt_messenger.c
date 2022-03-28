//
// Created by jordan on 2022-03-28.
//

#include "messenger.h"

int main()
{
    int shm_fd;
    char * msg;
    Messenger * messenger;
    MsgConsumer * consumer;

    messenger = messenger_init();
    if ( messenger )
    {
        consumer = consumer_init(messenger);
        if ( consumer )
        {
            shm_fd = consumer->fd_i;
            while ( consumer->sh_mem )
            {
                consumer->sh_mem = mmap(0, 4096, PROT_READ, MAP_SHARED, shm_fd, 0);
                msg = (char *)consumer->sh_mem;
                if ( strlen(msg) )
                {
                    printf("%s", (char *) consumer->sh_mem);
                    memset((char *) consumer->sh_mem, 0, strlen(consumer->sh_mem));
                }
            }

        } else { return EXIT_FAILURE; }

    } else { return EXIT_FAILURE; }

    shm_unlink(messenger->name);
    return 0;
}

