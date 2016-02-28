#include "vga.h"
#include "common.h"
#include "string.h"

u16 *vga_mem = (u16 *)VGA_MEM_START;

u32 x_pos = 0;
u32 y_pos = 0;

u32 translate(u32 x, u32 y) {
	return (y * VGA_WIDTH) + x;
}

void update_cursor() {
	u16 cur_pos = translate(x_pos, y_pos);
	outb(0x3D4, 14);
	outb(0x3D5, cur_pos >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, cur_pos);
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

void putc(char c) {
	if (c >= ' ') {
		vga_mem[translate(x_pos, y_pos)] = (0x07 << 8) | c;
		x_pos++;
	} else if (c == '\n') {
		x_pos = 0;
		y_pos++;
	}

	if (x_pos >= VGA_WIDTH) {
		x_pos = 0;
		y_pos++;
	}

	scroll();
	update_cursor();
}

void putco(char c, char color) {
	if (c >= ' ') {
		vga_mem[translate(x_pos, y_pos)] = (color << 8) | c;
		x_pos++;
	} else if (c == '\n') {
		x_pos = 0;
		y_pos++;
	}

	if (x_pos >= VGA_WIDTH) {
		x_pos = 0;
		y_pos++;
	}

	scroll();
	update_cursor();
}

void print(const char *str) {
	u32 len = 0;
	while(str[len] != 0) {
		putc(str[len]);
		len++;
	}
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
			putc(' ');
		}
	}
	x_pos = 0;
	y_pos = 0;
}

void putn(u32 i, u8 base) {
	char tbuf[32];
	char *buf = tbuf;

	if (base > 16) return;

	itoa(i, base, buf);
	print(buf);
}
