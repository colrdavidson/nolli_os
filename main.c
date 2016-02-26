typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef char i8;
typedef short i16;
typedef int i32;

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

u16 *vga_mem = (u16 *)0xB8000;
u32 x_pos = 0;
u32 y_pos = 0;

typedef struct {
	u32 base;
	u32 base_hi;
	u32 size;
	u32 size_hi;
	u32 type;
} __attribute__((packed)) mem_map_ptr;

mem_map_ptr *mem_map;
u16 mem_map_size;

void puts(const char *str);
void putn(u32);

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

void putn(u32 n) {
	if (n == 0) {
		putc('0');
		return;
	}

	char c[32];
	i32 acc = n;
	i32 i = 0;
	while (acc > 0) {
		c[i] = '0' + acc%10;
		acc /= 10;
		i++;
	}
	c[i] = '0';

	char c2[32];
	c2[i--] = 0;
	i32 j = 0;
	while(i >= 0) {
		c2[i--] = c[j++];
	}

	print(c2);
}

void kmain() {
	clear_screen();
	puts("Good news everyone!");
	print("The "); put_name(); puts(" kernel says hello!");

	for (int entry = 0; entry < mem_map_size; entry++) {
		print("mem_map base: "); putn(mem_map[entry].base); putc('\n');
		//print("mem_map base_hi: "); putn(mem_map[entry].base_hi); putc('\n');
		print("mem_map size: "); putn(mem_map[entry].size); putc('\n');
		//print("mem_map size_hi: "); putn(mem_map[entry].size_hi); putc('\n');
		print("mem_map type: "); putn(mem_map[entry].type); putc('\n');
		putc('\n');
	}
}
