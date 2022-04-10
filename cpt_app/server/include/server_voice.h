//
// Created by jordan on 2022-04-09.
//

#ifndef CPT_SERVER_VOICE_H
#define CPT_SERVER_VOICE_H

#include "pthread.h"
#include "voice_task.h"
#include "udp_server.h"
#include "server_global.h"

//TODO voice stuff here !!!

#define NUM_VTHREADS 4

/**
 *
 * @param task
 */
void execute_voice_task(VoiceTask * task);


/**
 *
 * @param task
 */
void submit_task(VoiceTask * task);


/**
 *
 * @param args
 * @return
 */
_Noreturn void * start_thread(void * T);


/**
 *
 * @param run
 * @return
 */
int voice_pool(void (run) (void));


/**
 *
 * @param uid
 */
void toggle_user_vchan(int uid);


/**
 *
 * @param uid
 * @param udp_fd_r
 */
void set_user_vchan(int uid, int udp_fd_r);


/**
 *
 * @param uid
 * @return
 */
int get_user_vchan(int uid);

#endif //CPT_SERVER_VOICE_H
