//
// Created by jordan on 2022-04-07.
//

#include "voice.h"
#include "tcp_server_config.h"

_Noreturn void record(int fd)
{
    char *cmd = "arecord -";
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
    FILE * file;

    file = fopen("./file.txt", "a+");

    while(true)
    {
        ssize_t nread;

//        nread = read(fd, buf, sizeof(buf));


        nread = udp_server_recv(fd, (uint8_t *)buf);
        printf("%zu\n", nread);
        printf("%s\n", buf);

        if(nread > 0)
        {
            fwrite(buf, 1, sizeof(buf), file);
            fwrite(buf, 1, sizeof(buf), fp);
//            fwrite(buf, sizeof(buf), nread, fp);
        }
        else
        {
            fwrite("nothin", 1, strlen("nothin") + 1, file);
            break;
        }
    }
    fclose(file);

    pclose(fp);
}


int run_voice_chat(const char * host, const char * port)
{
    pid_t pid;
    int listen_fd, send_fd;

    FILE * file2;
    if ( (pid = fork()) != -1 )
    {
        file2 = fopen("./file_outer.txt", "a+");
        if (pid > 0)
        {
            listen_fd = udp_server_sock_r("192.168.1.106", PORT_8888);
            play(listen_fd);
        }

        else
        {
            send_fd = udp_server_sock_s(host, port);
            record(send_fd);
        }
        wait(NULL);
        fclose(file2);
    }

    return pid;
}
