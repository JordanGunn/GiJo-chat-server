//
// Created by jordan on 2022-03-08.
//

#include "../include/cpt_builder.h"

/**
* Initialize cpt struct.
*
* Dynamically allocates a cpt struct and
* initializes all fields.
*
* @return Pointer to cpt struct.
*/
cpt_builder * cpt_builder_init()
{
    cpt_builder * cpt_builder;
    if ( (cpt_builder = malloc(sizeof(struct cpt_builder))) )
    {
        cpt_builder->msg = NULL;
    }

    return cpt_builder;
}



/**
* Free all memory and set fields to null.
*
* @param cpt   Pointer to a cpt structure.
*/
void cpt_builder_destroy(cpt_builder * cpt)
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
void cpt_builder_cmd(cpt_builder * cpt, uint8_t cmd)
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
void cpt_builder_version(cpt_builder * cpt, uint8_t version_major, uint8_t version_minor)
{
    bool exceeds_major;
    bool exceeds_minor;

    version = 0;
    exceeds_major = (version_major > VERSION_MAJOR_MAX);
    exceeds_minor = (version_minor > VERSION_MINOR_MAX);

    if ( exceeds_major || exceeds_minor) { return; }

    version_major <<= 4;
    cpt->version = version_major + version_minor;
}


/**
* Set the message length for the cpt header block.
*
* @param cpt       Pointer to a cpt structure.
* @param msg_len   An 8-bit integer.
*/
void cpt_builder_len(cpt_builder * cpt, uint8_t msg_len)
{
    cpt->msg_len = msg_len;
}


/**
* Set the channel id for the cpt header block.
*
* @param cpt           Pointer to a cpt structure.
* @param channel_id    A 16-bit integer.
*/
void cpt_builder_chan(cpt_builder * cpt, uint16_t channel_id)
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
void cpt_builder_msg(cpt_builder * cpt, char * msg)
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
* Create a cpt struct from a cpt packet.
*
* @param packet    A serialized cpt protocol message.
* @return          A pointer to a cpt struct.
*/
cpt_builder * cpt_builder_parse(uint8_t * packet)
{
    cpt_builder * cpt;
    cpt = cpt_builder_init();
    memset(cpt, 0, sizeof(struct cpt_builder));
    char msg_buff[SM_BUFF_SIZE];


    parse(
        packet, "CCHC256s",
            &cpt->version, &cpt->command, &cpt->channel_id,
            &cpt->msg_len, msg_buff
        );

    cpt->msg = (uint8_t *)strdup(msg_buff);

    return cpt;
}


/**
* Serialize a cpt_builder struct for transmission.
*
* @param cpt    A cpt_builder struct.
* @return       Size of the serialized packet.
*/
size_t cpt_builder_serialize(cpt_builder * cpt, uint8_t * buffer)
{
    size_t serial_size;

    serial_size = serialize(
          buffer, "CCHCs",
          (uint8_t)cpt->version, (uint8_t)cpt->command, (uint16_t)cpt->channel_id,
          (uint8_t)cpt->msg_len, cpt->msg
        );

    return serial_size;
}


/**
 * Print out the contents of a CPT packet
 *
 * @param cpt
 */
char * cpt_to_string(cpt_builder * cpt)
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


