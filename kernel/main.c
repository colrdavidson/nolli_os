#include "common.h"
#include "vga.h"

typedef struct {
	u32 base;
	u32 base_hi;
	u32 size;
	u32 size_hi;
	u32 type;
} __attribute__((packed)) mem_map_ptr;

mem_map_ptr *mem_map;
u16 mem_map_size;

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
	print("The "); put_name(); puts(" kernel says hello!\n");

	print_map();
}
