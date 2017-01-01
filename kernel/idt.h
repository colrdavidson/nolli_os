#ifndef _IDT_H
#define _IDT_H

#include "common.h"

typedef struct __attribute__((packed)) {
	u16 base_lo;
	u16 sel;
	u8 reserved;
	u8 flags;
	u16 base_hi;
} idt_entry_t;

typedef struct __attribute__((packed)) {
	u16 limit;
	u32 addr;
} idt_ptr_t;

idt_entry_t idt_entries[256];
idt_ptr_t idt_ptr;

extern void ex_c0();
extern void ex_c1();
extern void ex_c2();
extern void ex_c3();
extern void ex_c4();
extern void ex_c5();
extern void ex_c6();
extern void ex_c7();
extern void ex_e8();
extern void ex_c9();
extern void ex_e10();
extern void ex_e11();
extern void ex_e12();
extern void ex_e13();
extern void ex_e14();
extern void ex_c15();
extern void ex_c16();
extern void ex_c17();
extern void ex_c18();
extern void ex_c19();
extern void ex_c20();
extern void ex_c21();
extern void ex_c22();
extern void ex_c23();
extern void ex_c24();
extern void ex_c25();
extern void ex_c26();
extern void ex_c27();
extern void ex_c28();
extern void ex_c29();
extern void ex_c30();
extern void ex_c31();
extern void irq_0();
extern void irq_1();
extern void irq_2();
extern void irq_3();
extern void irq_4();
extern void irq_5();
extern void irq_6();
extern void irq_7();
extern void irq_8();
extern void irq_9();
extern void irq_10();
extern void irq_11();
extern void irq_12();
extern void irq_13();
extern void irq_14();
extern void irq_15();

extern void idt_load();

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
	set_intr(32, (u32)irq_0, 0x08, 0x8E);
	set_intr(33, (u32)irq_1, 0x08, 0x8E);
	set_intr(34, (u32)irq_2, 0x08, 0x8E);
	set_intr(35, (u32)irq_3, 0x08, 0x8E);
	set_intr(36, (u32)irq_4, 0x08, 0x8E);
	set_intr(37, (u32)irq_5, 0x08, 0x8E);
	set_intr(38, (u32)irq_6, 0x08, 0x8E);
	set_intr(39, (u32)irq_7, 0x08, 0x8E);
	set_intr(40, (u32)irq_8, 0x08, 0x8E);
	set_intr(41, (u32)irq_9, 0x08, 0x8E);
	set_intr(42, (u32)irq_10, 0x08, 0x8E);
	set_intr(43, (u32)irq_11, 0x08, 0x8E);
	set_intr(44, (u32)irq_12, 0x08, 0x8E);
	set_intr(45, (u32)irq_13, 0x08, 0x8E);
	set_intr(46, (u32)irq_14, 0x08, 0x8E);
	set_intr(47, (u32)irq_15, 0x08, 0x8E);

	idt_load();
	puts("initialized interrupts");

	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 4);
	outb(0xA1, 2);
	outb(0x21, 0x1);
	outb(0xA1, 0x1);

    // Enable Keyboard
	outb(0x21, 0xfd);
	outb(0xA1, 0xff);

	puts("enabled the keyboard\n");
}

#endif
