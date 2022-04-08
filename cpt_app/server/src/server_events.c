//
// Created by jordan on 2022-04-07.
//

#include "server_events.h"


void handle_event(ServerInfo * info, CptRequest * req)
{

    if ( req->command == SEND )
    {
        puts("  SEND event");
        send_event(info, (char *) req->msg, req->channel_id);
        cpt_request_destroy(req);
    }

    if ( req->command == GET_USERS )
    {
        puts("  GET_USERS event");
        get_users_event(info, req->channel_id);
        cpt_request_destroy(req);
    }

    if ( req->command == CREATE_CHANNEL )
    {
        puts("  CREATE_CHANNEL event");
        create_channel_event(info, (char *) req->msg);
        cpt_request_destroy(req);
    }

    if ( req->command == CREATE_VCHAN )
    {
        puts("  CREATE_VOICE_CHANNEL EVENT");
        create_vchannel_event(info, (char *) req->msg);
        cpt_request_destroy(req);
    }

    if ( req->command == LEAVE_CHANNEL )
    {
        puts("  LEAVE_CHANNEL event");
        leave_channel_event(info, req->channel_id);
        cpt_request_destroy(req);
    }

    if ( req->command == JOIN_CHANNEL )
    {
        puts("  JOIN_CHANNEL event");
        join_channel_event(info, req->channel_id);
        cpt_request_destroy(req);
    }
}


void join_channel_event(ServerInfo *info, uint16_t channel_id)
{
    char * msg;
    int join_res;
    size_t res_size;
    CptResponse * res;
    uint8_t res_buf[MD_BUFF_SIZE] = {0};

    join_res = cpt_join_channel_response(info, channel_id);
    res = cpt_response_init();
    if ( join_res == SUCCESS )
    { /* Send back confirmation if successful */
        res->code = (uint8_t) JOIN_CHANNEL;
        res->data_size = sizeof(channel_id);
        res->data = (uint8_t *) &channel_id;
    }
    else
    {
        msg = "Failed to join channel";
        res->code = (uint8_t) FAILURE;
        res->data_size = (uint16_t) strlen(msg);
        res->data = (uint8_t *) msg;
    }

    res_size = cpt_serialize_response(res, res_buf);
    tcp_server_send(info->current_id, res_buf, res_size);
    cpt_response_destroy(res);
}


void leave_channel_event(ServerInfo * info, uint16_t channel_id)
{
    int lc_res;
    char * res_msg;
    size_t res_size;
    CptResponse * res;
    uint8_t res_buf[MD_BUFF_SIZE] = {0};
    char res_msg_buf[SM_BUFF_SIZE] = {0};

    lc_res = cpt_leave_channel_response(info, channel_id);
    res = cpt_response_init();
    if ( lc_res == SUCCESS )
    { /* Send back confirmation if successful */
        sprintf(res_msg_buf,
                "Successfully left channel %d", channel_id);
        res_msg = strdup(res_msg_buf);
        res->code = LEAVE_CHANNEL;
    }
    else
    {
        res_msg = strdup("Failed to leave channel");
        res->code = FAILURE;
    }

    res->data_size = (uint16_t) strlen(res_msg);
    res->data = (uint8_t *) res_msg;
    res_size = cpt_serialize_response(res, res_buf);
    tcp_server_send(info->current_id, res_buf, res_size);
    cpt_response_destroy(res);
}


void logout_event(ServerInfo * info)
{
    int lo_res;
    lo_res = cpt_logout_response(info);

    if ( lo_res == SUCCESS )
    {
        printf("  User with ID %d logged out...\n", info->current_id);
    }
}


void create_channel_event(ServerInfo * info, char * id_list)
{
    char * msg;
    int cc_res;
    uint16_t ncid;
    size_t res_size;
    CptResponse * res;
    uint8_t res_buf[MD_BUFF_SIZE] = {0};

    cc_res = cpt_create_channel_response(info, id_list);
    res = cpt_response_init();

    ncid = ((uint16_t) (info->dir->length - 1));

    if ( cc_res == SUCCESS )
    {
        res->data = (uint8_t *) &ncid;
        res->data_size = sizeof(ncid);
        res->code = (uint8_t) CREATE_CHANNEL;
    }
    else
    {
        msg = strdup("Failed to create channel...");
        res->data = (uint8_t *) msg;
        res->data_size = sizeof(msg);
        res->code = (uint8_t) FAILURE;
    }

    res_size = cpt_serialize_response(res, res_buf);
    tcp_server_send(info->current_id, res_buf, res_size);
    cpt_response_destroy(res);
}


void create_vchannel_event(ServerInfo * info, char * id_list)
{
    char * msg;
    int cc_res;
    size_t res_size;
    uint8_t res_buf[MD_BUFF_SIZE] = {0};

    info->res = cpt_response_init();
    cc_res = cpt_create_vchannel_response(info, id_list);

    if ( cc_res == SUCCESS )
    {
        info->res->code = (uint8_t) CREATE_VCHAN;
    }
    else
    {
        msg = strdup("Failed to create channel...");
        info->res->data = (uint8_t *) msg;
        info->res->data_size = sizeof(msg);
        info->res->code = (uint8_t) FAILURE;
    }

    res_size = cpt_serialize_response(info->res, res_buf);
    tcp_server_send(info->current_id, res_buf, res_size);
    cpt_response_destroy(info->res);
}


void get_users_event(ServerInfo * info, int chan_id)
{
    int gu_res;
    size_t res_size;
    uint8_t res_buf[LG_BUFF_SIZE] = {0};

    info->res = cpt_response_init();
    gu_res = cpt_get_users_response(info, (uint16_t) chan_id);

    if ( gu_res == SUCCESS )
    {
        printf("GET_USERS event occurred successfully...\n");
    }

    res_size = cpt_serialize_response(info->res, res_buf);
    tcp_server_send(info->current_id, res_buf, res_size);
    cpt_response_destroy(info->res);
}


void send_event(ServerInfo * info, char * msg, int channel_id)
{
    User * user;
    size_t res_size;
    uint16_t dest_id;
    Channel * channel;
    UserNode * user_node;
    uint8_t res_buf[LG_BUFF_SIZE] = {0};

    channel = find_channel(info->dir, (uint16_t) channel_id);
    if ( channel )
    {
        user_node = get_head_user(channel->users);
        if ( user_node )
        {
            info->res = cpt_response_init();
            info->res->code = (uint8_t) SEND;
            info->res->data_size = (uint16_t) strlen(msg);
            info->res->data = (uint8_t *) strdup(msg);
            res_size = cpt_serialize_response(info->res, res_buf);

            while ( user_node )
            {
                user = user_node->user;
                dest_id = user->id;
                if ( dest_id != GC_ROOT_USR_ID )
                {
                    if ( dest_id != info->current_id )
                    {
                        tcp_server_send(dest_id, res_buf, res_size);
                    }
                }
                user_node = user_node->next_user;
            }
        }
    }

    cpt_response_destroy(info->res);
}
