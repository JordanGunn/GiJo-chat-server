//
// Created by jordan on 2022-03-22.
//

#include "handlers.h"
#include "shared_memory.h"


int login_handler(UserState * ustate, char * name)
{
    int result;
    CptResponse * res;
    ssize_t res_size, req_size;
    uint8_t res_buf[LG_BUFF_SIZE] = {0};
    uint8_t req_buf[LG_BUFF_SIZE] = {0};

    // send data to server
    req_size = (ssize_t) cpt_login(ustate->client_info, req_buf, name);
    result = tcp_client_send(ustate->client_info->fd, req_buf, (size_t) req_size);
    ustate->client_info->channel = CHANNEL_ZERO;
    ustate->client_info->name = strdup(name);

    if (result != SYS_CALL_FAIL)
    {
        res_size = (ssize_t) tcp_client_recv(ustate->client_info->fd, res_buf);
        res = cpt_parse_response(res_buf, (size_t) res_size);
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
            ustate->client_info, req_buf, ustate->cmd->args);

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
    ClientInfo * info;
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


void join_channel_handler(UserState * ustate)
{
    ssize_t n_sent;
    bool some_condition;

    size_t req_size;
    int result, udp_fd;
    uint16_t channel_id;
    char * args_end, * host, * port;
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

    host = ustate->client_info->ip;
    port = ustate->client_info->port;

    // TODO: VOICE FUNCTIONALITY GOES HERE.
    if ( is_voice(ustate) )
    {
        some_condition = true;
        udp_fd = udp_client_init(host, port);
        while ( some_condition )
        {  /* WE CAN INSERT ARECORD/APLAY RIGHT HERE */
            char * msg = "LOOK AT ALL THIS DATA";
            n_sent = udp_client_send(udp_fd, (uint8_t *) msg, strlen(msg));
            if ( n_sent < 0 ) { some_condition = false; }
        }
    }
}


void send_handler(UserState * ustate)
{
    int result;
    char * block;
    ssize_t req_size;
    uint16_t channel_id;
    char * msg_prefix, * name;
    char msg_buf[SM_BUFF_SIZE] = {0};
    uint8_t req_buf[MD_BUFF_SIZE] = {0};

    req_size = cpt_send(
            ustate->client_info, req_buf, ustate->cmd->input);

    result = tcp_client_send(
            ustate->client_info->fd, req_buf, (size_t) req_size);

    if ( result != SYS_CALL_FAIL )
    {
        name = ustate->client_info->name;
        channel_id = ustate->channel;
        sprintf(msg_buf, "[ %s | (channel %hu) ]: ", name, channel_id);
        block = shmem_attach(FILENAME, BLOCK_SIZE);
        strncat(msg_buf, ustate->cmd->input, strlen(ustate->cmd->input));
        msg_prefix = strdup(msg_buf);
        strncpy(block, msg_prefix, strlen(msg_prefix));
        shmem_detach(block);
    }
}


void recv_handler(UserState * ustate, const CptResponse * res)
{
    uint16_t cid;
    char * block;

    if ( res->code == (uint8_t) GET_USERS )
    {
        printf("%s\n", (char *)res->data);
    }

    if ( res->code == (uint8_t) CREATE_CHANNEL )
    {
        cid = (uint16_t) ( *(res->data) ); // new channel id is in response
        ustate->channel = cid;
        printf("\nSuccessfully created channel %d\n", cid);
    }

    if ( res->code == (uint8_t) JOIN_CHANNEL )
    {
        cid = (uint16_t) ( *(res->data) );
        ustate->channel = cid;
    }

    if ( res->code == (uint8_t) LEAVE_CHANNEL )
    {
        printf("%s\n", (char *)res->data);
        ustate->channel = CHANNEL_ZERO;
    }
    if ( res->code == (uint8_t) SEND )
    {
        block = shmem_attach(FILENAME, BLOCK_SIZE);
        strncpy(block, (char *) res->data, BLOCK_SIZE);
        shmem_detach(block);
    }
}


void cmd_handler(UserState * ustate)
{
    if ( is_cmd(ustate->cmd, cli_cmds[MENU_CMD]           )) { menu();                         }
    if ( is_cmd(ustate->cmd, cli_cmds[LOGOUT_CMD]         )) { logout_handler(ustate);         }
    if ( is_cmd(ustate->cmd, cli_cmds[GET_USERS_CMD]      )) { get_users_handler(ustate);      }
    if ( is_cmd(ustate->cmd, cli_cmds[CREATE_CHANNEL_CMD] )) { create_channel_handler(ustate); }
    if ( is_cmd(ustate->cmd, cli_cmds[JOIN_CHANNEL_CMD]   )) { join_channel_handler(ustate);   }
    if ( is_cmd(ustate->cmd, cli_cmds[LEAVE_CHANNEL_CMD]  )) { leave_channel_handler(ustate);  }
}


bool is_voice_compat(UserState * ustate)
{
    CptRequest * req;
    uint8_t ver_major;

    req = ustate->client_info->packet;
    ver_major = (req->version & ( MASK_8BIT_MSIG4 )) >> SHIFT4;

    return (ver_major >= VER_MAJ_2);
}


bool is_voice_chan(UserState * ustate)
{
    CptRequest * req;
    bool vchan_lbound, vchan_ubound;

    req = ustate->client_info->packet;

    vchan_lbound = (req->channel_id >= CPT_VCHAN_MIN);
    vchan_ubound = (req->channel_id <= CPT_VCHAN_MIN);

    return (vchan_lbound && vchan_ubound);
}


bool is_voice(UserState * ustate)
{
    bool is_vchan, is_vcompat;

    is_vchan = is_voice_chan(ustate);
    is_vcompat = is_voice_compat(ustate);

    return (is_vchan && is_vcompat);
}

