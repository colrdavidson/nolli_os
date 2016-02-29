#include "common.h"
#include "vga.h"

void outb(u16 port, u8 value) {
	asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

u8 inb(u16 port) {
	u8 rv;
	asm volatile ("inb %1, %0" : "=a" (rv) : "dN" (port));
	return rv;
}

void explode_if(u8 invalid) {
	if (invalid) {
		puts("OH NOES!");
	}
}
