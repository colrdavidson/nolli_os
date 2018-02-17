#ifndef VGA_H
#define VGA_H

#include "common.h"
#include "string.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEM_START 0xB8000

u16 *vga_mem = (u16 *)VGA_MEM_START;

u32 x_pos = 0;
u32 y_pos = 0;

u32 translate(u32 x, u32 y) {
	return (y * VGA_WIDTH) + x;
}

void update_cursor() {
	u16 cur_pos = translate(x_pos, y_pos);
	out_8(0x3D4, 14);
	out_8(0x3D5, cur_pos >> 8);
	out_8(0x3D4, 15);
	out_8(0x3D5, cur_pos);
}

void scroll() {
	if (y_pos >= VGA_HEIGHT) {
		u32 i;
		for (i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
			vga_mem[i] = vga_mem[i+80];
		}

		for (i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < (VGA_HEIGHT * VGA_WIDTH); i++) {
			vga_mem[i] = (0x07 << 8) | ' ';
		}

		y_pos = VGA_HEIGHT - 1;
	}
}

void _putc(char c, char color) {
	if (c >= ' ') {
		vga_mem[translate(x_pos, y_pos)] = (color << 8) | c;
		x_pos++;
	} else if (c == '\n') {
		x_pos = 0;
		y_pos++;
	} else if (c == '\b') {
		if (x_pos > 0) {
			x_pos--;
		} else {
			if (y_pos > 0) {
				y_pos--;
				x_pos = VGA_WIDTH - 1;
			}
		}
		vga_mem[translate(x_pos, y_pos)] = (color << 8) | ' ';
	} else if (c == '\t') {
		_putc(' ', color);
		_putc(' ', color);
		_putc(' ', color);
		_putc(' ', color);
	}

	if (x_pos >= VGA_WIDTH) {
		x_pos = 0;
		y_pos++;
	}

	scroll();
}

void putc(char c) {
	_putc(c, 0x7);
	update_cursor();
}

void aputc(char c) {
	_putc(c, 0x7);
}

void putco(char c, char color) {
	_putc(c, color);
	update_cursor();
}

void print(const char *str) {
	u32 len = 0;
	while(str[len] != 0) {
		aputc(str[len]);
		len++;
	}
	update_cursor();
}

void puts(const char *str) {
	print(str);
	putc('\n');
}

void put_name() {
	putco('N', 0x09);
	putco('o', 0x0B);
	putco('l', 0x0F);
	putco('l', 0x0C);
	putco('i', 0x0D);
	putco(' ', 0x00);
	putco('3', 0x06);
	putco('2', 0x06);
}

void clear_screen() {
	for (u32 x = 0; x < VGA_WIDTH; x++) {
		for (u32 y = 0; y < VGA_HEIGHT; y++) {
			aputc(' ');
		}
	}
	x_pos = 0;
	y_pos = 0;
	update_cursor();
}

void putn(u32 i, u8 base) {
	char tbuf[32];
	char *buf = tbuf;

	if (base > 16) return;

	itoa(i, base, buf);
	print(buf);
}

void printf(char *fmt, ...) {
	__builtin_va_list args;
	__builtin_va_start(args, fmt);

	for (char *c = fmt; *c != 0; c++) {
		if (*c != '%') {
			aputc(*c);
			continue;
		}

		c++;
		switch (*c) {
			case 'c': {
				char i = __builtin_va_arg(args, int);
				aputc(i);
			} break;
			case 's': {
				char *s = __builtin_va_arg(args, char *);
				print(s);
			} break;
			case 'd': {
				s32 i = __builtin_va_arg(args, int);
				putn(i, 10);
			} break;
			case 'x': {
				s32 i = __builtin_va_arg(args, int);
				putn(i, 16);
			} break;
			case 'b': {
				s32 i = __builtin_va_arg(args, int);
				putn(i, 2);
			} break;
			case 'p': {
				s32 i = __builtin_va_arg(args, int);
				aputc('0');
				aputc('x');
				putn(i, 16);
			} break;
			case 'n': {
				put_name();
			}
		}
	}

	update_cursor();
	__builtin_va_end(args);
}

#endif
