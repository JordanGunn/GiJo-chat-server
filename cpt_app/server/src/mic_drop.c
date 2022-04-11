//
// Created by Giwoun Bae on 2022-04-09.
//

#include "mic_drop.h"

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>

Task taskQueue[256];
int taskCount = 0;

pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

#define VOICE_BUF_SIZE 256

void executeTask(Task* task) {
    char buf[VOICE_BUF_SIZE];
    ssize_t nread;
    Users * users;
    UserNode * userNode;

    nread = 0;
    users = task->users;
    userNode = get_head_user(users);

    puts("receiving voice from UDP");
    UserNode * next_user;
    while ((nread = recv(task->my_fd, buf, VOICE_BUF_SIZE, 0)) > 0)
    {
        while ( userNode )
        {
            next_user = userNode->next_user;
            printf("Sending to listener %d\n", userNode->user->voice_fd);
            if ( userNode->user->voice_fd != task->my_fd)
            {
                send( userNode->user->voice_fd , buf, VOICE_BUF_SIZE, 0);
            }
            userNode = next_user;
        }
    }
}

void submitTask(Task task) {
    pthread_mutex_lock(&mutexQueue);
    taskQueue[taskCount] = task;
    taskCount++;
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
}

void* startThread(void* args) {
    while (1) {
        Task task;

        pthread_mutex_lock(&mutexQueue);
        while (taskCount == 0) {
            pthread_cond_wait(&condQueue, &mutexQueue);
        }

        task = taskQueue[0];
        int i;
        for (i = 0; i < taskCount - 1; i++) {
            taskQueue[i] = taskQueue[i + 1];
        }
        taskCount--;
        pthread_mutex_unlock(&mutexQueue);
        executeTask(&task);
    }
}


int run_voice(void) {
    pthread_t th[THREAD_NUM];
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);
    int i;
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_create(&th[i], NULL, &startThread, NULL) != 0) {
            perror("Failed to create the thread");
        }
    }

    //TODO creating tasks. Need serverInfo, and currentChannel.
    ServerInfo *serverInfo = NULL;
    Channel* currentChannel = NULL;
    Task* t = create_voice_task(serverInfo, currentChannel);
    submitTask(*t);
    //TODO end of function that can be used.

    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join the thread");
        }
    }
    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);
    return 0;
}

Task * channel_task_init()
{
    Task * task;

    task = malloc(sizeof(struct cpt_request));
    if ( task )
    {
        task->my_fd = 0;
        task->users = NULL;
    }

    return task;
}


Task* create_voice_task(ServerInfo *info, Channel *channel)
{
    User *user;
    Task * ct;

    ct = channel_task_init();
    user = find_user(channel->users, info->current_id);
    ct->my_fd = user->voice_fd;
    ct->uid = info->current_id;
    ct->users = channel->users;
    return ct;
}

void destroy_voice_task( Task * task )
{
    if ( task )
    {
        free(task);
        task = NULL;
    }
}

