//
// Created by jordan on 2022-03-08.
//

#include "cpt_packet_builder.h"


CptPacket * cpt_request_init()
{
    CptPacket * cpt_packet;
    if ( (cpt_packet = malloc(sizeof(struct cpt_packet))) )
    {
        cpt_packet->msg = NULL;
    }

    return cpt_packet;
}



void cpt_request_destroy(CptPacket * cpt)
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


void cpt_request_cmd(CptPacket * cpt, uint8_t cmd)
{
    if ( (cmd >= SEND) && (cmd <= LOGIN) )
    {
        cpt->command = cmd;
    } else { printf("Bad CMD code assigned to packet\n"); }
}


void cpt_request_version(CptPacket * cpt, uint8_t version_major, uint8_t version_minor)
{
    bool exceeds_major;
    bool exceeds_minor;

    exceeds_major = (version_major > VERSION_MAJOR_MAX);
    exceeds_minor = (version_minor > VERSION_MINOR_MAX);

    if ( exceeds_major || exceeds_minor) { return; }

    version_major <<= (uint8_t) 4;
    cpt->version = version_major + version_minor;
}


void cpt_request_chan(CptPacket * cpt, uint16_t channel_id)
{
    cpt->channel_id = channel_id;
}


void cpt_request_msg(CptPacket * cpt, char * msg)
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


void cpt_request_reset(CptPacket * packet)
{
    if ( packet->msg ) { free(packet->msg); packet->msg = NULL; }
    packet->msg_len = 0;
    packet->command = 0;
}


char * cpt_to_string(CptPacket * cpt)
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


// ==================================
// C P T   S E R V E R  P A C K E T S
// ==================================


CptResponse * cpt_response_init(uint16_t res_code)
{
    CptResponse * res;

    if ( !(res = malloc(sizeof(struct cpt_response))) ) { return NULL; }

    res->code = res_code;
    res->data = NULL;

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


void cpt_response_reset(CptResponse * res)
{
    if ( res->data )
    {
        free(res->data);
        res->data = NULL;
    }

    res->code = 0;
}




// ============================
// C P T   S U B - P A CK E T S
// ============================


CptMsgSubPacket * cpt_msg_sp_init()
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
