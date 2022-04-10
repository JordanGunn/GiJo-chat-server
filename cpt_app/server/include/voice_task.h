//
// Created by jordan on 2022-04-09.
//

#ifndef CPT_VOICE_TASK_H
#define CPT_VOICE_TASK_H


#include "channel.h"
#include "common.h"


/**
 * Voice task object.
 */
typedef struct voice_task VoiceTask;
struct voice_task {
    int uid;
    int udp_fd;
    Channel * channel;
};


/**
 * Initialize voice task object.
 *
 * @return Pointer to VoiceTask object.
 */
VoiceTask * voice_task_init(void);


/**
 *
 *
 * @param voice_task
 */
void voice_task_destroy(VoiceTask * voice_task);


#endif //CPT_VOICE_TASK_H
