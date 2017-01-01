#include "common.h"
#include "vga.h"
#include "mem.h"
#include "isr.h"
#include "idt.h"

void kmain() {
	clear_screen();
	printf("Good news everyone!\n");
	printf("The %n kernel says hello!\n\n");

	init_idt();
	init_mem();

    asm ("sti");

	for (;;) {
		asm ("hlt");
	}
}
