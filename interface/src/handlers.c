//
// Created by jordan on 2022-03-22.
//

#include "handlers.h"
#include "shared_memory.h"


int login_handler(UserState * ustate, char * name)
{
    int result;
    CptResponse * res;
    size_t res_size, req_size;
    uint8_t res_buf[LG_BUFF_SIZE] = {0};
    uint8_t req_buf[LG_BUFF_SIZE] = {0};

    // send data to server
    req_size = cpt_login(ustate->client_info, req_buf, name);
    result = tcp_client_send(ustate->client_info->fd, req_buf, req_size);
    ustate->client_info->channel = CHANNEL_ZERO;
    ustate->client_info->name = strdup(name);

    if (result != SYS_CALL_FAIL)
    {
        res_size = tcp_client_recv(ustate->client_info->fd, res_buf);
        res = cpt_parse_response(res_buf, res_size);
        if ( res )
        {
            if (res->code == (uint8_t) LOGIN)
            {
                printf("\n\nLogged in to chat server as %s@%s:%s\n\n",
                       name, ustate->client_info->ip, ustate->client_info->port);
            }
            else
            {
                printf("Error from server with code %d\n", res->code);
            }
            cpt_response_destroy(res);
        }
    }

    return result;
}


void logout_handler(UserState * ustate)
{
    size_t req_size;
    uint8_t req_buf[LG_BUFF_SIZE] = {0};

    req_size = cpt_logout(ustate->client_info, req_buf);
    tcp_client_send(ustate->client_info->fd, req_buf, req_size);
    ustate->LOGGED_IN = false;
}


void create_channel_handler(UserState * ustate)
{
    int result;
    size_t req_size;
    uint8_t req_buf[MD_BUFF_SIZE] = {0};

    req_size = cpt_create_channel(
            ustate->client_info, req_buf, (char *) ustate->cmd->args);

    result = tcp_client_send(
            ustate->client_info->fd, req_buf, req_size);

    if ( result == SYS_CALL_FAIL )
    {
        printf("Failed to SEND create channel request\n");
    }
}


void get_users_handler(UserState * ustate)
{
    int result;
    char * args_end;
    size_t req_size;
    uint16_t channel_id;
    CptClientInfo * info;
    uint8_t req_buf[MD_BUFF_SIZE] = {0};

    info = ustate->client_info;
    channel_id = ( ustate->cmd->args )
                 ? (uint16_t) strtol( ustate->cmd->args, &args_end, 10)
                 : ustate->channel;

    req_size = cpt_get_users(
            info, req_buf, channel_id);

    result = tcp_client_send(
            info->fd, req_buf, req_size);

    if ( result == SYS_CALL_FAIL )
    {
        printf("Failed to send GET_USERS request\n");
    }
}


void leave_channel_handler(UserState * ustate)
{
    int result;
    size_t req_size;
    uint8_t req_buf[MD_BUFF_SIZE] = {0};

    req_size = cpt_leave_channel(
            ustate->client_info, req_buf, ustate->channel);

    result = tcp_client_send(
            ustate->client_info->fd, req_buf, req_size);

    if ( result == SYS_CALL_FAIL )
    {
        printf("Failed to send LEAVE_CHANNEL request\n");
    }
}



void join_channel_handler(UserState * ustate) {

    int result;
    char * args_end;
    uint16_t channel_id;
    size_t req_size;
    uint8_t req_buf[MD_BUFF_SIZE] = {0};

    channel_id = (uint16_t) strtol(ustate->cmd->args, &args_end, 10);

    req_size = cpt_join_channel(
            ustate->client_info, req_buf, channel_id);

    result = tcp_client_send(
            ustate->client_info->fd, req_buf, req_size);

    if ( result == SYS_CALL_FAIL )
    {
        printf("Failed to send JOIN_CHANNEL request\n");
    }
}


void send_handler(UserState * ustate)
{
    int result;
    char * block;
    size_t req_size;
    uint8_t req_buf[MD_BUFF_SIZE] = {0};

    req_size = cpt_send(
            ustate->client_info, req_buf, ustate->cmd->input);

    result = tcp_client_send(
            ustate->client_info->fd, req_buf, req_size);

    if ( result != SYS_CALL_FAIL )
    {
        block = shmem_attach(FILENAME, BLOCK_SIZE);
        strncpy(block, (char *) ustate->cmd->input, BLOCK_SIZE);
        shmem_detach(block);
    }
}





