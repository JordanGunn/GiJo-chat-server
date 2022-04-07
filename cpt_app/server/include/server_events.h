//
// Created by jordan on 2022-04-07.
//

#ifndef CPT_SERVER_EVENTS_H
#define CPT_SERVER_EVENTS_H

#include "server_config.h"
#include "tcp_server.h"
#include "cpt_server.h"


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


/**
 * Handle all incoming events.
 *
 * @param info
 */
void handle_event(ServerInfo * info, CptRequest * req);



#endif //CPT_SERVER_EVENTS_H
