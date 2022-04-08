//
// Created by jordan on 2022-03-08.
//

#include "cpt_packet_builder.h"

// ========================================
//  C P T   R E Q U E S T   P A C K E T
// ========================================
CptRequest * cpt_request_init()
{
    CptRequest * req;

    req = malloc(sizeof(struct cpt_request));
    if ( req )
        { req->msg = NULL; }

    return req;
}


void cpt_request_destroy(CptRequest * req)
{
    if (req)
    {
        if (req->msg)
            { free(req->msg); req->msg = NULL; }

        free(req);
        req = NULL;
    }
}


void cpt_request_cmd(CptRequest * cpt, uint8_t cmd)
{
    if ( (cmd >= SEND) && (cmd <= CREATE_VCHAN) )
    {
        cpt->command = cmd;
    } else { printf("Bad CMD code assigned to packet\n"); }
}


void cpt_request_version(CptRequest * cpt, uint8_t version_major, uint8_t version_minor)
{
    bool exceeds_major;
    bool exceeds_minor;

    exceeds_major = (version_major > VERSION_MAJOR_MAX);
    exceeds_minor = (version_minor > VERSION_MINOR_MAX);

    if ( exceeds_major || exceeds_minor) { return; }

    version_major <<= (uint8_t) 4;
    cpt->version = version_major + version_minor;
}


void cpt_request_chan(CptRequest * cpt, uint16_t channel_id)
{
    cpt->channel_id = channel_id;
}


void cpt_request_msg(CptRequest * cpt, char * msg)
{
    char * msg_field;

    msg_field = NULL;
    if (msg)
    {
        msg_field = strdup(msg);
    }

    if (msg_field)
    {
        cpt->msg = (uint8_t *)msg_field;
        cpt->msg_len = strlen(msg);
    }
}


void cpt_request_reset(CptRequest * packet)
{
    if ( packet->msg )
    {
        free(packet->msg);
        packet->msg = NULL;
    }
    packet->msg_len = 0;
    packet->command = 0;
    packet->channel_id = 0;
}


char * cpt_to_string(CptRequest * cpt)
{
    char * msg, * cpt_str;
    int cmd, msg_len, chan_id;
    char buffer[MD_BUFF_SIZE] = {0};
    uint8_t version_minor, version_major;

    version_major = cpt->version;
    version_minor = cpt->version;
    version_major >>= (uint8_t) 4;
    version_minor &= (uint8_t) 15;

    cmd     = cpt->command;
    msg_len = cpt->msg_len;
    chan_id = cpt->channel_id;

    msg     = ( cpt->msg )
        ? strdup((char *)cpt->msg)
        : strdup("(empty)");

    sprintf(buffer,
        "VERSION: %d.%d\n"  \
        "COMMAND: %d\n"     \
        "CHAN ID: %d\n"     \
        "MSG LEN: %d\n"     \
        "MSG: \"%s\"\n\n",  \
        version_major, version_minor,
        cmd, chan_id, msg_len, msg
    );

    cpt_str = strdup(buffer);
    free(msg); msg = NULL;
    return cpt_str;
}


// =======================================
//  C P T   R E S P O N S E    P A C K E T
// =======================================
CptResponse * cpt_response_init(void)
{
    CptResponse * res;

    if ( !(res = malloc(sizeof(struct cpt_response))) ) { return NULL; }
    memset(res, 0, sizeof(struct cpt_response));
    res->code = 0;
    res->data = NULL;

    return res;
}


void cpt_response_destroy(CptResponse * res)
{
    if ( res )
        { free(res); res = NULL; }
}


void cpt_response_reset(CptResponse * res)
{
    if ( res->data )
    {
        free(res->data);
        res->data = NULL;
    }

    res->data_size = 0;
    res->code = 0;
}



// ============================
// C P T   S U B - P A CK E T S
// ============================
CptMsgSubPacket * cpt_msg_sp_init(void)
{
    size_t num_bytes;
    CptMsgSubPacket * msg_res;

    num_bytes = sizeof(struct cpt_msg_sub_packet);
    if ( !(msg_res = malloc(num_bytes)) ) { return NULL; }

    return msg_res;
}


void cpt_msg_sp_destroy(CptMsgSubPacket * msg_res)
{
    if ( msg_res )
    {
        if ( msg_res->msg )
        {
            free(msg_res->msg);
            msg_res->msg = NULL;
        }
        free(msg_res);
        msg_res = NULL;
    }
}
