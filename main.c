typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

u16 *vga_mem = (u16 *)0xB8000;
u32 x_pos = 0;
u32 y_pos = 0;

void outb(u16 port, u8 value) {
	asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

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

	update_cursor();
}

void puts(const char *str) {
	u32 len = 0;
	while(str[len] != 0) {
		putc(str[len]);
		len++;
	}
	putc('\n');
}

void print(const char *str) {
	u32 len = 0;
	while(str[len] != 0) {
		putc(str[len]);
		len++;
	}
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

void kmain() {
	clear_screen();
	puts("Good news everyone!");
	print("The "); put_name(); puts(" kernel says hello!");
}
