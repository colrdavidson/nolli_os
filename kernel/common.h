#ifndef _COMMON_H
#define _COMMON_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef char i8;
typedef short i16;
typedef int i32;

typedef u8 bool;
#define true 1
#define false 0
#define NULL 0

static const char bchars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void outb(u16 port, u8 value) {
	asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

void out_16(u16 port, u16 value) {
	asm volatile ("out %1, %0" : : "dN" (port), "a" (value));
}

void out_32(u16 port, u32 value) {
	asm volatile ("outl %1, %0" : : "dN" (port), "a" (value));
}

u8 inb(u16 port) {
	u8 rv;
	asm volatile ("inb %1, %0" : "=a" (rv) : "dN" (port));
	return rv;
}

u8 in_16(u16 port) {
	u16 rv;
	asm volatile ("in %1, %0" : "=a" (rv) : "dN" (port));
	return rv;
}

u8 in_32(u16 port) {
	u32 rv;
	asm volatile ("inl %1, %0" : "=a" (rv) : "dN" (port));
	return rv;
}

#include "vga.h"

void explode_if(u8 invalid) {
	if (invalid) {
		puts("[BOOM] OH NOES!");
		asm ("hlt");
	}
}

#endif
