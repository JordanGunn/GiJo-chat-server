//
// Created by jordan on 2022-04-09.
//

#include "server_info.h"
#include "server_voice.h"

//TODO voice stuff here !!!

int task_count;
pthread_cond_t cond_queue;
pthread_mutex_t mutex;
VoiceTask * voice_task_queue[1024];
int voice_udp_fds_r[VOICE_MAX_CHAN];
bool active_voice_chan[VOICE_MAX_CHAN];

void execute_voice_task(VoiceTask * task)
{
    Users * users;
    ssize_t voice_size;
    UserNode * user_node, * next_user;
    uint8_t voice_buf[SM_BUFF_SIZE] = {0};

    users = task->channel->users;
    user_node = get_head_user(users);
    while ( (voice_size = udp_server_recv(task->udp_fd, voice_buf)) > 0 )
    {
        while (user_node)
        {
            next_user = user_node->next_user;
            if ( task->uid != user_node->user->id )
            {
                udp_server_send(user_node->user->udp_fd_s,
                                voice_buf, (size_t) voice_size);
            }
            user_node = next_user;
        }
    }
}


void submit_task(VoiceTask * task)
{
    pthread_mutex_lock(&mutex);

    voice_task_queue[task_count] = task;
    task_count++;

    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond_queue);
}


_Noreturn void * start_thread(void * T)
{
    int ind;
    VoiceTask * task;

    while ( true )
    {
        pthread_mutex_lock(&mutex);
        while (task_count == 0) {
            pthread_cond_wait(&cond_queue, &mutex);
        }

        task = voice_task_queue[0];

        for (ind = 0; ind < task_count - 1; ind++) {
            voice_task_queue[ind] = voice_task_queue[ind + 1];
        }
        task_count--;

        pthread_mutex_unlock(&mutex);
        if ( active_voice_chan[task->uid] )
        {
            execute_voice_task(task);
            submit_task(task);
        }
        else
        {
            voice_task_destroy(task);
            task = NULL;
        }
    }
}

int voice_pool(void (run) (void))
{
    int ind;
    pthread_t th[NUM_VTHREADS];
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_queue, NULL);
    for (ind = 0; ind < NUM_VTHREADS; ind++) {
        if (pthread_create(&th[ind], NULL, &start_thread, NULL) != 0) {
            perror("Failed to create the thread");
        }
    }

    run(); /* run the server */

    for (ind = 0; ind < NUM_VTHREADS; ind++) {
        if (pthread_join(th[ind], NULL) != 0) {
            perror("Failed to join the thread");
        }
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_queue);
    return 0;
}


void toggle_user_vchan(int uid)
{
    pthread_mutex_lock(&mutex);
    active_voice_chan[uid] = !active_voice_chan[uid];
    pthread_mutex_unlock(&mutex);
}


void set_user_vchan(int uid, int udp_fd_r)
{
    pthread_mutex_lock(&mutex);
    voice_udp_fds_r[uid] = udp_fd_r;
    pthread_mutex_unlock(&mutex);
}


int get_user_vchan(int uid)
{
    int udp_fd_r;

    pthread_mutex_lock(&mutex);
    udp_fd_r = voice_udp_fds_r[uid];
    pthread_mutex_unlock(&mutex);

    return udp_fd_r;
}
