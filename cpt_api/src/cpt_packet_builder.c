//
// Created by jordan on 2022-03-08.
//

#include "cpt_packet_builder.h"

/**
* Initialize cpt struct.
*
* Dynamically allocates a cpt struct and
* initializes all fields.
*
* @return Pointer to cpt struct.
*/
CptPacket * cpt_packet_init()
{
    CptPacket * cpt_packet;
    if ( (cpt_packet = malloc(sizeof(struct cpt_packet))) )
    {
        cpt_packet->msg = NULL;
    }

    return cpt_packet;
}



/**
* Free all memory and set fields to null.
*
* @param cpt   Pointer to a cpt structure.
*/
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


/**
* Set the command value for the cpt header block.
*
* @param cpt   Pointer to a cpt structure.
* @param cmd   From enum commands.
*/
void cpt_packet_cmd(CptPacket * cpt, uint8_t cmd)
{
    if ( (cmd < SEND) || (cmd > LEAVE_CHANNEL) )
    {
        cpt->command = cmd;
    }
}


/**
* Set major and minor version for the cpt header block.
*
* @param cpt           Pointer to a cpt structure.
* @param version_major From enum version.
* @param version_minor From enum version.
*/
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


/**
* Set the channel id for the cpt header block.
*
* @param cpt           Pointer to a cpt structure.
* @param channel_id    A 16-bit integer.
*/
void cpt_packet_chan(CptPacket * cpt, uint16_t channel_id)
{
    cpt->channel_id = channel_id;
}


/**
* Set the MSG field for the cpt packet and
* appropriately update the MSG_LEN
*
* @param cpt  Pointer to a cpt structure.
* @param msg  Pointer to an array of characters.
*/
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


/**
 * Reset packet parameters.
 *
 * Reset the packet parameters,
 * and free memory for certain params.
 *
 * @param cpt    A CptPacket struct.
*/
void cpt_packet_reset(CptPacket * cpt)
{
    if ( cpt->msg ) { free(cpt->msg); cpt->msg = NULL; }
    cpt->msg_len = 0;
    cpt->command = 0;
}


/**
 * Print out the contents of a CPT packet
 *
 * @param cpt
 */
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


CptResponse * cpt_response_init(uint16_t fd, uint16_t res_code, uint8_t * data)
{
    CptResponse * res;

    if ( !(res = malloc(sizeof(struct cpt_response))) ) { return NULL; }
    if ( !data ) { return NULL; }

    res->buffer = (uint8_t *) strdup((char *) data);
    res->code = res_code;
    res->fd = fd;

    return res;
}


void cpt_response_destroy(CptResponse * res)
{
    if ( res )
    {
        if ( res->buffer )
        {
            free(res->buffer);
            res->buffer = NULL;
        }
        free(res);
        res = NULL;
    }
}



// ============================
// C P T   S U B - P A CK E T S
// ============================


CptMsgResponse * cpt_msg_response_init(uint8_t * msg, uint16_t chan_id, uint16_t user_id)
{
    CptMsgResponse * msg_res;

    if ( !(msg_res = malloc(sizeof(struct cpt_msg_response))) ) { return NULL; }
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


