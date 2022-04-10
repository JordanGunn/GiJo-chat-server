//
// Created by jordan on 2022-04-09.
//

#include "voice_task.h"


/**
 * Initialize voice task object.
 *
 * @return Pointer to VoiceTask object.
 */
VoiceTask * voice_task_init()
{
    VoiceTask * voice_task;

    if ( (voice_task = malloc(sizeof(struct voice_task))) )
    {
        voice_task->channel = NULL;
        voice_task->udp_fd = 0;
        voice_task->uid = 0;
    }

    return voice_task;
}


/**
 *
 *
 * @param voice_task
 */
void voice_task_destroy(VoiceTask * voice_task)
{
    if ( voice_task )
    {
        if ( voice_task->channel )
        {
            free(voice_task->channel);
            voice_task->channel = NULL;
        }
        free(voice_task);
        voice_task = NULL;
    }
}
