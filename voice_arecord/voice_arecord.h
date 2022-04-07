//
// Created by root on 2022-04-06.
//

#ifndef CPT_VOICE_ARECORD_H
#define CPT_VOICE_ARECORD_H


#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/**
 * Recording the voice
 *
 * @param fd
 */
void record(int fd);

/**
 * Play the voice
 * @param fd
 */
void play(int fd);

#endif //CPT_VOICE_ARECORD_H
