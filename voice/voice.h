//
// Created by jordan on 2022-04-07.
//

#ifndef CPT_VOICE_H
#define CPT_VOICE_H

#include "udp_client.h"

_Noreturn void record(int fd);
void play(int fd);
int run_voice_chat(const char * host, const char * port);


#endif //CPT_VOICE_H
