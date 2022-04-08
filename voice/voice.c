//
// Created by jordan on 2022-04-07.
//

#include "voice.h"

_Noreturn void record(int fd)
{
    char *cmd = "arecord - -q";
    char buf[256];
    FILE *fp = popen(cmd, "r");

    while (true)
    {
        size_t result;

        result = fread(buf, 1, sizeof(buf), fp);
        write(fd, buf, result);
    }

    pclose(fp);
}


void play(int fd)
{
    char *cmd = "aplay -";
    char buf[256];
    FILE *fp = popen(cmd, "w");

    while(true)
    {
        ssize_t nread;

        nread = read(fd, buf, sizeof(buf));
        printf("%zu\n", nread);

        if(nread > 0)
        {
            fwrite(buf, 1, sizeof(buf), fp);
        }
        else
        {
            break;
        }
    }

    pclose(fp);
}


int run_voice_chat(const char * host, const char * port)
{
    pid_t pid;
    int listen_fd, send_fd;

    listen_fd = udp_client_init(host, port);
    send_fd = udp_client_init(host, port);
    if ((pid = fork()) != -1)
    {
        if (pid == 0)
        { play(listen_fd); }

        else
        { record(send_fd); }
    }

    return pid;
}
