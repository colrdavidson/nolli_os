#include "serial.h"
#include "common.h"
#include "string.h"

#define PORT 0x3f8

void init_serial() {
	outb(PORT + 1, 0x00);
	outb(PORT + 3, 0x80);
	outb(PORT + 0, 0x03);
	outb(PORT + 1, 0x00);
	outb(PORT + 3, 0x03);
	outb(PORT + 2, 0xC7);
	outb(PORT + 4, 0x0B);
}

i32 is_transmit_empty() {
	return inb(PORT + 5) & 0x20;
}

void sputc(char c) {
	while (is_transmit_empty() == 0);
	outb(PORT, c);
}

void sprint(const char *str) {
	u32 len = 0;
	while(str[len] != 0) {
		sputc(str[len]);
		len++;
	}
}

void sputs(const char *str) {
	sprint(str);
	sputc('\n');
}

void sputn(i32 i, u8 base) {
	char tbuf[32];
	char *buf = tbuf;

	if (base > 16) return;
	if (i < 0) {
		*buf++ = '-';
		i *= -1;
	}

	itoa((u32)i, base, buf);
	sprint(buf);
}
