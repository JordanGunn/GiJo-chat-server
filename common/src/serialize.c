//
// Created by jordan on 2022-03-17.
//

#include "serialize.h"


void packi16(unsigned char *buf, unsigned int i)
{
    *buf++ = i>>8; *buf++ = i;
}


unsigned int serialize(unsigned char * buf, char * format, ...)
{
    va_list ap;

    signed char c;              // 8-bit
    unsigned char C;

    unsigned int H;

    char * s;                    // strings
    unsigned int len;

    unsigned int size = 0;

    va_start(ap, format);
    for(; *format != '\0'; format++) {
        switch(*format) {
            case 'c': // 8-bit
                size += 1;
                c = (signed char)va_arg(ap, int); // promoted
                *buf++ = c;
                break;

            case 'C': // 8-bit unsigned
                size += 1;
                C = (unsigned char)va_arg(ap, unsigned int); // promoted
                *buf++ = C;
                break;

            case 'H': // 16-bit unsigned
                size += 2;
                H = va_arg(ap, unsigned int);
                packi16(buf, H);
                buf += 2;
                break;

            case 's': // string
                s = va_arg(ap, char*);
                len = strlen(s);
                size += len + 2;
                packi16(buf, len);
                buf += 2;
                memcpy(buf, s, len);
                buf += len;
                break;
        }
    }
    va_end(ap);
    return size;
}


void parse(unsigned char * buffer, char * format, ...)
{
    va_list ap;

    signed char *c;              // 8-bit
    unsigned char *C;

    int *h;                      // 16-bit
    unsigned int *H;

    char *s;
    unsigned int len, maxstrlen=0, count;

    va_start(ap, format);

    for(; *format != '\0'; format++) {
        switch(*format) {

            case 'c': // 8-bit
                c = va_arg(ap, signed char*);
                if (*buffer <= 0x7f) { *c = *buffer;} // re-sign
                else { *c = -1 - (unsigned char)(0xffu - *buffer); }
                buffer++;
                break;

            case 'C': // 8-bit unsigned
                C = va_arg(ap, unsigned char*);
                *C = *buffer++;
                break;

            case 'h': // 16-bit
                h = va_arg(ap, int*);
                *h = unpacki16(buffer);
                buffer += 2;
                break;

            case 'H': // 16-bit unsigned
                H = va_arg(ap, unsigned int*);
                *H = unpacku16(buffer);
                buffer += 2;
                break;

            case 's': // string
                s = va_arg(ap, char * );
                len = unpacku16(buffer);
                buffer += 2;
                if (maxstrlen > 0 && len > maxstrlen) count = maxstrlen - 1;
                else count = len;
                memcpy(s, buffer, count);
                s[count] = '\0';
                buffer += len;
                break;

            default:
                if (isdigit(*format)) { // track max str len
                    maxstrlen = maxstrlen * 10 + (*format-'0');
                }
        }

        if (!isdigit(*format)) maxstrlen = 0;
    }

    va_end(ap);
}


int unpacki16(unsigned char * buf)
{
    unsigned int i2 = ((unsigned int)buf[0]<<8) | buf[1];
    int i;

    // change unsigned numbers to signed
    if (i2 <= 0x7fffu) { i = i2; }
    else { i = -1 - (0xffffu - i2); }

    return i;
}


unsigned int unpacku16(unsigned char * buf)
{
    return ((unsigned int)buf[0] << 8) | buf[1];
}

