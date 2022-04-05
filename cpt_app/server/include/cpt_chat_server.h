//
// Created by jordan on 2022-03-08.
//

#ifndef CPT_CPT_CHAT_SERVER_H
#define CPT_CPT_CHAT_SERVER_H

#include "server_config.h"
#include "tcp_server.h"

#include "cpt_server.h"


/**
 * Drive the program.
 */
void run();


/**
 *
 * @return
 */
int handle_new_accept();


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



// ==========================================
//  S E R V E R    E V E N T S
// ==========================================

/**
 *
 * @param info
 * @return
 */
int login_event(ServerInfo * info);


/**
 *
 * @param info
 */
void logout_event(ServerInfo * info);


/**
 *
 * @param info
 * @param chan_id
 */
void get_users_event(ServerInfo * info, int chan_id);


/**
 *
 * @param info
 * @param msg
 */
void send_message_event(ServerInfo * info, char * msg);


/**
 *
 * @param info
 * @param id
 */
void leave_channel_event(ServerInfo * info, uint16_t id);


/**
 *
 * @param info
 * @param id_list
 */
void create_channel_event(ServerInfo * info, char * id_list);


/**
 *
 * @param info
 * @param channel_id
 */
void join_channel_event(ServerInfo *info, uint16_t channel_id);


/**
 *
 * @param info
 * @param msg
 * @param channel_id
 */
void send_event(ServerInfo * info, char * msg, int channel_id);




#endif //CPT_CPT_CHAT_SERVER_H
