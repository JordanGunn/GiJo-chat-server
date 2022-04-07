//
// Created by root on 2022-04-06.
//

#include "voice_arecord.h"

void record(int fd)
{
//    char *cmd = "arecord -";
    char *cmd = "rec";
    char buf[256];
    FILE *fp = popen(cmd, "r");

    while(true)
    {
        size_t result;

        result = fread(buf, 1, sizeof(buf), fp);
        write(fd, buf, result);
    }

    pclose(fp);
}


void play(int fd)
{
//    char *cmd = "aplay -";
    char *cmd = "play";
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
