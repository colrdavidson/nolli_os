#include "common.h"
#include "vga.h"
#include "mem.h"
#include "serial.h"

void kmain() {
	init_serial();

	clear_screen();
	puts("Good news everyone!");
	print("The "); put_name(); puts(" kernel says hello!\n");

	print_map();
	init_mem();
}
