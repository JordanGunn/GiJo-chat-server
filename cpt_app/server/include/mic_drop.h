//
// Created by Giwoun Bae on 2022-04-09.
//

#ifndef CPT_MIC_DROP_H
#define CPT_MIC_DROP_H

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "channel.h"
#include "server_info.h"

#define THREAD_NUM 100

typedef struct Task {
    int my_fd;
    Users* users;
} Task;

/**
 * Executing task of sending voice packets from a user to all other user
 * in the voice channel.
 *
 * @param task a task object
 */
void executeTask(Task* task);

/**
 * Submit a task to the task queue.
 *
 * @param task
 */
void submitTask(Task task);

/**
 * A thread function to initialize a thread as
 *  voice server thread to transmit voice from
 *  one user to other users in the same channel.
 *
 * @param args
 * @return
 */
void* startThread(void* args);

int run_voice(void);

/**
 * @return
 */
Task * channel_task_init(void);

Task* create_voice_task(ServerInfo *info, Channel *channel);

void destroy_voice_task( Task * task );


#endif //CPT_MIC_DROP_H
