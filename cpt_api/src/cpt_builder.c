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
void cpt_builder_msg(cpt_builder * cpt, const char * msg)
{
    char * msg_field;

    msg_field = NULL;
    if (msg)
    {
        msg_field = strdup(msg);
    }

    if (msg_field)
    {
        cpt->msg = msg_field;
        cpt->msg_len = strlen(cpt->msg);
    }
}


/**
* Create a cpt struct from a cpt packet.
*
* @param packet    A serialized cpt protocol message.
* @return          A pointer to a cpt struct.
*/
cpt_builder * cpt_builder_parse(void * packet)
{
    uint8_t * to_parse;
    cpt_builder * cpt;
    cpt = cpt_builder_init();
    const char * format_spec;

    format_spec = "cls";
    to_parse = (uint8_t *)packet;

    parse(
        to_parse, format_spec,
            cpt->version, cpt->command, cpt->channel_id,
            cpt->msg_len, cpt->msg
        );

    puts("fuck yeah");
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
    const char * format_spec;

    format_spec = "cls";
    printf("packet size before serialization: %zu\n", sizeof(*cpt));

    serial_size = serialize(
          buffer, format_spec,
              cpt->version, cpt->command, cpt->channel_id,
              cpt->msg_len, cpt->msg
        );

    printf("packet size after serialization: %zu\n", serial_size);
    return serial_size;
}

void cpt_to_string(cpt_builder * cpt)
{
    char buffer[MD_BUFF_SIZE] = {0};
    uint8_t version_minor, version_major;
    version_major = cpt->version;
    version_minor = cpt->version;
    version_major >>= 4;
    version_minor &= 15;

    sprintf(buffer,
        "VERSION: %d.%d\n"  \
        "COMMAND: %d\n"     \
        "CHAN ID: %d\n"     \
        "MSG LEN: %d\n"     \
        "MSG: \"%s\"\n\n",     \
        version_major, version_minor,
        cpt->command, cpt->channel_id,
        cpt->msg_len, cpt->msg
    );

    printf("%s\n", buffer);
}


static void pack_uint16(uint8_t * serial_buffer, uint16_t int_16)
{
    *serial_buffer++ = int_16 >> 8;
    *serial_buffer++ = int_16;
}

static uint16_t unpack_uint16t(const uint8_t * serial_buffer)
{
   uint16_t unpacked;

   unpacked = (serial_buffer[0] << 8) | serial_buffer[1];
   return unpacked;
}

static uint16_t serialize(uint8_t * buffer, const char * format, ...)
{
    va_list argv;           // variable argument list
    uint8_t c;              // char
    uint16_t l;             // long (not actually... don't @ me)
    char * s;               // for strings
    uint16_t str_len;       // for strings
    uint16_t size = 0;      // for strings
    va_start(argv, format);

    for (; *format != '\0'; format++)
    {
        switch (*format) /* map variable arg format specifiers */
        {
            case 'c':
                size += 1;
                c = (unsigned char) va_arg(argv, unsigned int);
                *buffer++ = c;
            break;
            case 'l':
                size += 2;
                l = va_arg(argv, unsigned int);
                pack_uint16(buffer, l);
                buffer += 2;
            break;
            case 's':
                s = va_arg(argv, char*);
                str_len = strlen(s);
                size += str_len + 2;
                pack_uint16(buffer, str_len);
                buffer += 2;
                memcpy(buffer, s, str_len);
                buffer += str_len;
            break;
        }
    }
    va_end(argv);
    return size;
}

static void parse(uint8_t * buffer, const char * format, ...)
{
    va_list argv;
    unsigned char * uint8; // uint8_t
    unsigned int *  uint16; // uint16_t
    char * str;
    uint16_t str_len, count, max_len;

    max_len = 0;
    va_start(argv, format);
    for(; * format != '\0'; format++)
    {
        switch(*format)
        {
            case 'c': // 8-bit unsigned
                uint8 = va_arg(argv, unsigned char*);
                *uint8 = *buffer++;
            break;
            case 'l': // 16-bit unsigned
                uint16 = va_arg(argv, unsigned int*);
                *uint16 = unpack_uint16t(buffer);
                buffer += 2;
            break;
            case 's': // string
                str = va_arg(argv, char*);
                str_len = unpack_uint16t(buffer);
                buffer += 2;
                count = ( (max_len > 0) && (str_len >= max_len) )
                    ? max_len - 1 : str_len;
                memcpy(str, buffer, count);
                str[count] = '\0';
                buffer += str_len;
            break;
            default:
                if ( isdigit(*format) )
                {
                    max_len = max_len * 10 + (*format - '0');
                }
        }
        if ( !isdigit(*format) ) { max_len = 0 ;}
    }
    va_end(argv);
}

//
//
///**
//* Check serialized cpt to see if it is a valid cpt block.
//*
//* @param packet    A serialized cpt protocol message.
//* @return          0 if no issues, otherwise CPT error code.
//*/
//int cpt_validate(void * packet)
//{
//
//}
