//
// Created by jordan on 2022-03-08.
//

#ifndef CPT_CPT_CHAT_SERVER_H
#define CPT_CPT_CHAT_SERVER_H

#include "server_config.h"
#include "tcp_server.h"
#include "channel.h"
#include "user.h"

#include "cpt_types.h"
#include "cpt_server.h"

void run();


int login_event(CptServerInfo * info);

void logout_event(CptServerInfo * info);

void leave_channel_event(CptServerInfo * info, uint16_t id);

void create_channel_event(CptServerInfo * info, char * id_list);

void get_users_event(CptServerInfo * info, int chan_id);


bool is_revent_POLLIN(int index);

void listen_socket_init(Channel * gc);

int handle_new_accept();

void server_destroy(Channels * dir);

bool should_end_event_loop(int poll_result);




#endif //CPT_CPT_CHAT_SERVER_H
