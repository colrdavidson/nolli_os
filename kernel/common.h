#ifndef _COMMON_H
#define _COMMON_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef char i8;
typedef short i16;
typedef int i32;

static const char bchars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

u8 inb(u16 port);
void outb(u16 port, u8 value);

#endif
