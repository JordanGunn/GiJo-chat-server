//
// Created by root on 2022-04-06.
//

#include "voice_arecord.h"



//can I connect send and receive on the same socket?
//yes

void run_voice_chat(int fd)
{
    pid_t pid;


    pid = fork();

    if (pid == 0)
    {
        play(fd); //outgoing
    }
    else if (pid > 0)
    {
        record(fd); //incoming
    }
}



void record(int fd)
{
    char *cmd = "arecord -";
//    char *cmd = "/rec";
    char buf[256];
    FILE *fp = popen(cmd, "r");

    while(true)
    {
        size_t result;

        result = fread(buf, 1, sizeof(buf), fp);
        sendto(fd, buf, result, 0, NULL, NULL);
    }

    pclose(fp);
}


void play(int fd)
{
    char *cmd = "aplay -";
//    char *cmd = "play";
    char buf[256];
    FILE *fp = popen(cmd, "w");


    while(true)
    {
        ssize_t nread;
//        recvfrom(fd, buf, sizeof(buf), 0, NULL, NULL);
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
