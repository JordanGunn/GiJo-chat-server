//
// Created by jordan on 2022-03-08.
//

#ifndef CPT_CPT_CHAT_SERVER_H
#define CPT_CPT_CHAT_SERVER_H

#include "server_events.h"
#include "server_voice.h"
#include "udp_server.h"
#include "voice_task.h"
#include "channel.h"
#include "common.h"

/**
 * Drive the program.
 */
void run(void);


/**
 *
 * @param index
 * @return
 */
bool is_revent_POLLIN(int index);


/**
 *
 * @param gc
 */
void listen_socket_init(Channel * gc);


/**
 *
 * @param poll_result
 * @return
 */
bool should_end_event_loop(int poll_result);


/**
 * Compress fd array.
 */
void compress_fds(void);



/**
 * Close dangling fd connections.
 */
void close_connections(void);


#endif //CPT_CPT_CHAT_SERVER_H
