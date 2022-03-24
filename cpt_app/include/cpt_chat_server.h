//
// Created by jordan on 2022-03-08.
//

#ifndef CPT_CPT_CHAT_SERVER_H
#define CPT_CPT_CHAT_SERVER_H

#include "tcp_server.h"
#include "server_config.h"

#include "cpt_types.h"
#include "cpt_server.h"

void run();

bool is_revent_POLLIN(int index);

int login_event(CptServerInfo * info);

void logout_event(CptServerInfo * info);

void create_channel_event(CptServerInfo * info, char * id_list);

void get_users_event(CptServerInfo * info, int chan_id);

void listen_socket_init(Channel * gc);

int handle_new_accept();

void server_destroy(Channels * dir);

bool should_end_event_loop(int poll_result);




#endif //CPT_CPT_CHAT_SERVER_H
