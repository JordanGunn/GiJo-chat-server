//
// Created by jordan on 2022-03-08.
//

#ifndef CPT_CPT_CHAT_SERVER_H
#define CPT_CPT_CHAT_SERVER_H

#include "tcp_server.h"
#include "server_config.h"

#include "cpt_types.h"
#include "cpt_server.h"
#include "cpt_global_channel.h"

void run();

void server_destroy();

int login_event();

void listen_socket_init();

int handle_new_accept();

void logout_event(CptPacket *req, int id);

bool should_end_event_loop(int poll_result);




#endif //CPT_CPT_CHAT_SERVER_H
