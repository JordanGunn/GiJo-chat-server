//
// Created by jordan on 2022-03-08.
//

#ifndef CPT_CPT_CHAT_SERVER_H
#define CPT_CPT_CHAT_SERVER_H

#include "server_events.h"

/**
 * Drive the program.
 */
void run(void);


/**
 *
 * @param info
 * @return
 */
int login_event(ServerInfo * info);


/**
 *
 * @return
 */
int handle_new_accept(void);


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



// ==========================================
//  S E R V E R    E V E N T S
// ==========================================



/**
 * Close dangling fd connections.
 */
void close_connections(void);




#endif //CPT_CPT_CHAT_SERVER_H
