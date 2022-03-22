//
// Created by jordan on 2022-03-17.
//

/**
 * NOTE: This is not my original work. This was taken and slightly adapted from
 * the book:
 *      Beej’s Guide to Network Programming Using Internet Sockets
 *      by Brian “Beej Jorgensen” Hall,
 *      v3.1.5, November 20, 2020
 *
 * Another note. To anyone who made read this, this book is fantastic and is
 * publicly available for free.
 */


#ifndef CPT_SERIALIZE_H
#define CPT_SERIALIZE_H

#include <stdarg.h>
#include <string.h>
#include <ctype.h>

void packi16(unsigned char *buf, unsigned int i);
unsigned int unpacku16(unsigned char *buf);
int unpacki16(unsigned char *buf);


unsigned int serialize(unsigned char * buf, char * format, ...);
void parse(unsigned char * buffer, char * format, ...);

#endif //CPT_SERIALIZE_H
