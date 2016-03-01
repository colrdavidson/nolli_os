#include "common.h"
#include "vga.h"
#include "mem.h"
#include "serial.h"
#include "idt.h"

void kmain() {
	init_serial();

	clear_screen();
	puts("Good news everyone!");
	print("The "); put_name(); puts(" kernel says hello!");

	init_idt();
	init_mem();

	sprint("idt address: 0x"); sputn(idt_ptr.addr, 16); sputc('\n');
	//asm volatile ("int $0xA");
}
