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

void init_idt();

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


#endif
