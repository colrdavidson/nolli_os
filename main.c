typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef char i8;
typedef short i16;
typedef int i32;

char bchars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

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
void putn(i32, u8);

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

void itoa(u32 i, u8 base, char *buf) {
   int pos = 0;
   int o_pos = 0;
   int top = 0;
   char tbuf[32];

   if (i == 0 || base > 16) {
      buf[0] = '0';
      buf[1] = '\0';
      return;
   }

   while (i != 0) {
      tbuf[pos] = bchars[i % base];
      pos++;
      i /= base;
   }
   top = pos--;
   for (o_pos = 0; o_pos < top; pos--, o_pos++) {
      buf[o_pos] = tbuf[pos];
   }
   buf[o_pos] = 0;
}

void putn(i32 i, u8 base) {
	char tbuf[32];
	char *buf = tbuf;

	if (base > 16) return;
	if (i < 0) {
		*buf++ = '-';
		i *= -1;
	}

	itoa((u32)i, base, buf);
	print(buf);
}

void print_map() {
	for (int entry = 0; entry < mem_map_size; entry++) {
		print("mem_map["); putn(entry, 10); print("] range: 0x"); putn(mem_map[entry].base, 16); print(" - 0x");
		putn(mem_map[entry].base + mem_map[entry].size, 16); putc('\n');

		u32 type = mem_map[entry].type;
		print("mem_map["); putn(entry, 10);
		if (type != 1) { print("] type: Reserved"); putc('\n'); }
		else { print("] type: Useable"); putc('\n'); }

		putc('\n');
	}
}

void kmain() {
	clear_screen();
	puts("Good news everyone!");
	print("The "); put_name(); puts(" kernel says hello!");
	puts("");

	print_map();
}
