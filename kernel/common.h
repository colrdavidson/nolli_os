#ifndef _COMMON_H
#define _COMMON_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t  s16;
typedef int32_t s32;
typedef int64_t s64;

typedef u8 bool;
#define true 1
#define false 0
#define NULL 0

extern u32 get_eip();

static const char bchars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void out_8(u16 port, u8 value) {
	asm ("outb %1, %0" : : "dN" (port), "a" (value));
}

void out_16(u16 port, u16 value) {
	asm volatile ("out %1, %0" : : "dN" (port), "a" (value));
}

void out_32(u16 port, u32 value) {
	asm volatile ("outl %%eax, %%dx" : : "dN" (port), "a" (value));
}

u8 in_8(u16 port) {
	u8 rv;
	asm volatile ("inb %1, %0" : "=a" (rv) : "dN" (port));
	return rv;
}

u16 in_16(u16 port) {
	u16 rv;
	asm volatile ("inw %1, %0" : "=a" (rv) : "dN" (port));
	return rv;
}

u32 in_32(u16 port) {
	u32 rv;
	asm volatile ("inl %%dx, %%eax" : "=a" (rv) : "dN" (port));
	return rv;
}

void memcpy(u8 *mem1, u8 *mem2, u32 size) {
	for (u32 i = 0; i < size; i++) {
		mem2[i] = mem1[i];
	}
}

#include "vga.h"

u32 live_ticks = 0;
void sleep(u32 total_ticks) {
	u32 start_ticks = live_ticks;
	u32 elapsed_ticks = 0;
	while (elapsed_ticks < total_ticks) {
		asm ("hlt");
		elapsed_ticks = live_ticks - start_ticks;
	}
}

void halt_kernel() {
	x_pos = (VGA_WIDTH / 2) - 7;
	y_pos = VGA_HEIGHT / 2;
	printf("KERNEL PANIC");

	asm ("cli");
	asm ("hlt");
}

void pretty_dump(u16 *addr, u32 size) {
	u32 cur_inc = 0;
	u32 screen_size = VGA_WIDTH * VGA_HEIGHT * sizeof(u16);

	u32 total_pages = (size / screen_size) + 1;
	while ((cur_inc * screen_size) < size) {
		clear_screen();

		for (u32 i = 0; i < (screen_size / 2); i++) {
			((u16 *)VGA_MEM_START)[i] = addr[(cur_inc * screen_size) + i];
		}

		printf("[%p of %p] - %p\n", cur_inc + 1, total_pages, addr + (cur_inc * screen_size));
		sleep(10);
		cur_inc += 1;
	}
}

void explode_if(u8 invalid) {
	if (invalid) {
		int cur_eip = get_eip();
		pretty_dump((u16 *)cur_eip, 1);
		halt_kernel();
	}
}

#endif
