#ifndef _SERIAL_H
#define _SERIAL_H

#include "common.h"

#define PORT 0x3f8

void init_serial() {
	out_8(PORT + 1, 0x00);
	out_8(PORT + 3, 0x80);
	out_8(PORT + 0, 0x03);
	out_8(PORT + 1, 0x00);
	out_8(PORT + 3, 0x03);
	out_8(PORT + 2, 0xC7);
	out_8(PORT + 4, 0x0B);

	//sputs("\x1b[33m"); Make serial output orange/yellow
}

s32 is_transmit_empty() {
	return in_8(PORT + 5) & 0x20;
}

void sputc(char c) {
	while (is_transmit_empty() == 0);
	out_8(PORT, c);
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

void sputn(s32 i, u8 base) {
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

void sprintf(char *fmt, ...) {
	__builtin_va_list args;
	__builtin_va_start(args, fmt);

	for (char *c = fmt; *c != 0; c++) {
		if (*c != '%') {
			sputc(*c);
			continue;
		}

		c++;
		switch (*c) {
			case 'c': {
				char i = __builtin_va_arg(args, int);
				sputc(i);
			} break;
			case 's': {
				char *s = __builtin_va_arg(args, char *);
				sprint(s);
			} break;
			case 'd': {
				s32 i = __builtin_va_arg(args, int);
				sputn(i, 10);
			} break;
			case 'x': {
				s32 i = __builtin_va_arg(args, int);
				sputn(i, 16);
			} break;
			case 'b': {
				s32 i = __builtin_va_arg(args, int);
				sputn(i, 2);
			} break;
			case 'p': {
				s32 i = __builtin_va_arg(args, int);
				sputc('0');
				sputc('x');
				sputn(i, 16);
			} break;
		}
	}

	__builtin_va_end(args);
}

#endif
