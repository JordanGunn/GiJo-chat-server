//
// Created by jordan on 2022-03-19.
//

#ifndef CPT_CLI_DC_H
#define CPT_CLI_DC_H

#include "client_config.h"
#include "tcp_client.h"

#include  "application_framework.h"
#include "shared_memory.h"
#include "user_state.h"
#include "handlers.h"
#include "pthread.h"

#define NUM_MSG_THREADS 2

/**
 * Call the login_handler() function and control result.
 *
 * @param host  Server host address.
 * @param port  Server host port.
 * @param login Name of requested login user.
 */
void user_login(UserState * ustate, char * host, char * port, char * name);


/**
 * Run chat threads.
 *
 * Executes the recv thread and the send thread.
 *
 * @param th    An array of pthread objects.
 * @param th    Pointer to a UserState object.
 */
void thread_chat_io(pthread_t th[NUM_MSG_THREADS], UserState * ustate);



/**
 * Execute the receive thread for the application.
 *
 * Listens for incoming data on the network socket.
 * Locks complimentary send_thread if any data is received
 * and signals the condition variable before revealing the
 * cmd prompt again.
 *
 * @param user_state    Pointer to a UserState object.
 */
void * recv_thread(void * user_state);


/**
 * Execute the send thread for the application.
 *
 * Handles out going CPT messages generate from user input.
 * Waits for signal from recv_thread before revealing the prompt again.
 *
 * @param user_state    Pointer to a UserState object.
 */
void * send_thread(void * user_state);


/**
 * Print command prompt to stdout.
 *
 * @param ustate    UserState object.
 */
void prompt(UserState * ustate);


#endif //CPT_CLI_DC_H
