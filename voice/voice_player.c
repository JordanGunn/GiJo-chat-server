
#include "udp_client.h"
#include "voice.h"

int main(int arc, char * argv[])
{
    int listen_fd = 0;
    int send_fd = 0;
    pid_t pid;
    int pipe_fds[2];
    pipe(pipe_fds);

    if (arc < 3)
    {
        puts("Missing some arguments.");
        return EXIT_FAILURE;
    }
    puts("Music Player running..");

    listen_fd = udp_client_init(argv[1], argv[2]);
    send_fd = udp_client_init(argv[1], argv[2]);

    pid = fork();

    if (pid == 0)
    {
//        close(pipe_fds[1]);
//        play(pipe_fds[0]);

        play(listen_fd);
     } else
    {
//        close(pipe_fds[0]);
//        record(pipe_fds[1]);
        record(send_fd);
    }

    return EXIT_SUCCESS;
}
