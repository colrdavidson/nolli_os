#include "common.h"
#include "vga.h"
#include "mem.h"
#include "isr.h"
#include "idt.h"
#include "pci.h"
#include "eth.h"

void kmain() {
	clear_screen();
	printf("Good news everyone!\n");
	printf("The %n kernel says hello!\n\n");

	init_idt();
	init_mem();

    asm ("sti");

	pci_read_devices();
	PCI_dev *eth_dev = hook_device(0x8086, 0x100E);
	init_eth(eth_dev);

	for (;;) {
		asm ("hlt");
	}
}
