//
// Created by giwoun on 2022-04-06.
//
#include "voice_arecord.h"

#define TEST_HOST "192.168.1.79"
#define TEST_PORT "8080"

int main(void)
{
    struct adderinfo* client;
    int fd = 0;

    client = setup_client_addr(TEST_HOST, TEST_PORT);
    fd = create_client_socket(client);

    run_voice_chat(fd);
    return EXIT_SUCCESS;
}
