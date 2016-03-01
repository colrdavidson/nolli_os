#include "idt.h"
#include "common.h"
#include "vga.h"
#include "serial.h"

void flush(idt_ptr_t *iptr) {
	asm volatile("lidt (%0)" :: "r"(iptr->addr));
}

void set_intr(u8 num, u32 base, u16 sel, u8 flags) {
	idt_entries[num].base_lo = base & 0xFFFF;
	idt_entries[num].base_hi = (base >> 16) & 0xFFFF;
	idt_entries[num].sel = sel;
	idt_entries[num].reserved = 0;
	idt_entries[num].flags = flags;
}

void init_idt() {
	idt_ptr.limit = (sizeof(idt_entry_t) * 256) - 1;
	idt_ptr.addr = (u32)&idt_entries;

	print("1st handler address: 0x"); putn((u32)ex_c0, 16); putc('\n');
	set_intr(0,  (u32)ex_c0,  0x08, 0x8E);
	set_intr(1,  (u32)ex_c1,  0x08, 0x8E);
	set_intr(2,  (u32)ex_c2,  0x08, 0x8E);
	set_intr(3,  (u32)ex_c3,  0x08, 0x8E);
	set_intr(4,  (u32)ex_c4,  0x08, 0x8E);
	set_intr(5,  (u32)ex_c5,  0x08, 0x8E);
	set_intr(6,  (u32)ex_c6,  0x08, 0x8E);
	set_intr(7,  (u32)ex_c7,  0x08, 0x8E);
	set_intr(8,  (u32)ex_e8,  0x08, 0x8E);
	set_intr(9,  (u32)ex_c9,  0x08, 0x8E);
	set_intr(10, (u32)ex_e10, 0x08, 0x8E);
	set_intr(11, (u32)ex_e11, 0x08, 0x8E);
	set_intr(12, (u32)ex_e12, 0x08, 0x8E);
	set_intr(13, (u32)ex_e13, 0x08, 0x8E);
	set_intr(14, (u32)ex_e14, 0x08, 0x8E);
	set_intr(15, (u32)ex_c15, 0x08, 0x8E);
	set_intr(16, (u32)ex_c16, 0x08, 0x8E);
	set_intr(17, (u32)ex_c17, 0x08, 0x8E);
	set_intr(18, (u32)ex_c18, 0x08, 0x8E);
	set_intr(19, (u32)ex_c19, 0x08, 0x8E);
	set_intr(20, (u32)ex_c20, 0x08, 0x8E);
	set_intr(21, (u32)ex_c21, 0x08, 0x8E);
	set_intr(22, (u32)ex_c22, 0x08, 0x8E);
	set_intr(23, (u32)ex_c23, 0x08, 0x8E);
	set_intr(24, (u32)ex_c24, 0x08, 0x8E);
	set_intr(25, (u32)ex_c25, 0x08, 0x8E);
	set_intr(26, (u32)ex_c26, 0x08, 0x8E);
	set_intr(27, (u32)ex_c27, 0x08, 0x8E);
	set_intr(28, (u32)ex_c28, 0x08, 0x8E);
	set_intr(29, (u32)ex_c29, 0x08, 0x8E);
	set_intr(30, (u32)ex_c30, 0x08, 0x8E);
	set_intr(31, (u32)ex_c31, 0x08, 0x8E);

	flush(&idt_ptr);
	puts("initialized interrupts");
}
