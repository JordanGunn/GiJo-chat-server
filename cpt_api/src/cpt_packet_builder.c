//
// Created by jordan on 2022-03-08.
//

#include "cpt_packet_builder.h"


CptPacket * cpt_packet_init()
{
    CptPacket * cpt_packet;
    if ( (cpt_packet = malloc(sizeof(struct cpt_packet))) )
    {
        cpt_packet->msg = NULL;
    }

    return cpt_packet;
}



void cpt_packet_destroy(CptPacket * cpt)
{
    if (cpt)
    {
        if (cpt->msg)
        {
            free(cpt->msg);
            cpt->msg = NULL;
        }
        free(cpt);
        cpt = NULL;
    }
}


void cpt_packet_cmd(CptPacket * cpt, uint8_t cmd)
{
    if ( (cmd >= SEND) && (cmd <= LOGIN) )
    {
        cpt->command = cmd;
    } else { printf("Bad CMD code assigned to packet\n"); }
}


void cpt_packet_version(CptPacket * cpt, uint8_t version_major, uint8_t version_minor)
{
    bool exceeds_major;
    bool exceeds_minor;

    exceeds_major = (version_major > VERSION_MAJOR_MAX);
    exceeds_minor = (version_minor > VERSION_MINOR_MAX);

    if ( exceeds_major || exceeds_minor) { return; }

    version_major <<= 4;
    cpt->version = version_major + version_minor;
}


void cpt_packet_chan(CptPacket * cpt, uint16_t channel_id)
{
    cpt->channel_id = channel_id;
}


void cpt_packet_msg(CptPacket * cpt, char * msg)
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
        cpt->msg_len = strlen((char *)cpt->msg);
    }
}


void cpt_packet_reset(CptPacket * packet)
{
    if ( packet->msg ) { free(packet->msg); packet->msg = NULL; }
    packet->msg_len = 0;
    packet->command = 0;
}


char * cpt_to_string(CptPacket * cpt)
{
    char buffer[MD_BUFF_SIZE] = {0};
    uint8_t version_minor, version_major;
    version_major = cpt->version;
    version_minor = cpt->version;
    version_major >>= 4;
    version_minor &= 15;
    int cmd, msg_len, chan_id;
    char * msg;
    char * cpt_str;

    cmd     = (cpt->command >= 0   ) ? cpt->command              : -1;
    chan_id = (cpt->channel_id >= 0) ? cpt->channel_id           : -1;
    msg_len = (cpt->msg_len >= 0   ) ? cpt->msg_len              : -1;
    msg     = (cpt->msg            ) ? strdup((char *)cpt->msg)  : strdup("(empty)");

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
    return cpt_str;
}


// ==================================
// C P T   S E R V E R  P A C K E T S
// ==================================


CptResponse * cpt_response_init(uint16_t fd, uint16_t res_code)
{
    CptResponse * res;

    if ( !(res = malloc(sizeof(struct cpt_response))) ) { return NULL; }

    res->code = res_code;
    res->data = NULL;
    res->fd = fd;

    return res;
}


void cpt_response_destroy(CptResponse * res)
{
    if ( res )
    {
        if ( res->data )
        {
            free(res->data);
            res->data = NULL;
        }
        free(res);
        res = NULL;
    }
}


void cpt_response_reset(CptResponse * response)
{
    if ( response->data ) { free(response->data); response->data = NULL; }
    response->fd = 0;
    response->code = 0;
}




// ============================
// C P T   S U B - P A CK E T S
// ============================


CptMsgResponse * cpt_msg_response_init(uint8_t * msg, uint16_t chan_id, uint16_t user_id)
{
    size_t num_bytes;
    CptMsgResponse * msg_res;

    num_bytes = sizeof(struct cpt_msg_response);
    if ( !(msg_res = malloc(num_bytes)) ) { return NULL; }
    if ( !msg ) { return NULL; }

    msg_res->msg = (uint8_t *) strdup((char *) msg);
    msg_res->channel_id = chan_id;
    msg_res->channel_id = user_id;

    return msg_res;
}


void cpt_msg_response_destroy(CptMsgResponse * msg_res)
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
