#include "mem.h"
#include "common.h"
#include "vga.h"
#include "serial.h"

typedef struct __attribute__((packed)) {
	u32 base;
	u32 base_hi;
	u32 size;
	u32 size_hi;
	u32 type;
} mem_map_ptr;

typedef struct __attribute__((packed)) {
	i32 page_table;
	i32 page;
} page_info;

u32 bitmap_size;
u32 *memory;

mem_map_ptr *mem_map;
u16 mem_map_size;

u32 *p_page_dir;
u32 first_hole;

u32 set_bit(u32 map, u8 nbit) {
	return map | (1 << nbit);
}

u32 clear_bit(u32 map, u8 nbit) {
	return map ^ (1 << nbit);
}

i32 is_set(u32 map, u8 nbit) {
	return !!(map & (1 << nbit));
}

i32 index_of(u32 i) {
	return i / (sizeof(u32) * 8);
}

i32 offset_from(u32 i) {
	return i % (sizeof(u32) * 8);
}

void print_bitset(u32 idx) {
	print("set 0x"); putn(idx, 16); print(": ");
	putn(memory[idx], 2);
	putc('\n');
}

u32 alloc_p() {
	for (u32 i = first_hole; i < bitmap_size; i++) {
		if (!is_set(memory[index_of(i)], offset_from(i))) {
			memory[index_of(i)] = set_bit(memory[index_of(i)], offset_from(i));
			first_hole = i;
			return i * 0x1000;
		}
	}

	return 0;
}

void free(u32 addr, u32 size) {
	u32 iaddr = addr / 0x1000;
	for (u32 i = iaddr; i < (iaddr + size); i++) {
		memory[index_of(i)] = clear_bit(memory[index_of(i)], offset_from(i));
	}
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

u32 new_pde() {
	u32 addr = alloc_p();
	u32 pde = (addr & ~0xFFF) | 3; // Chop off the bottom 12 bits of the address, and then set as present and r/w
	return pde;
}

u32 new_pte() {
	u32 addr = alloc_p();
	u32 pte = (addr & ~0xFFF) | 3; // Chop off the bottom 12 bits of the address, and then set as present and r/w
	//print("new page at: 0x"); putn(addr & ~0xFFF, 16); putc('\n');
	return pte;
}

void init_mem() {
	bitmap_size = (mem_map[mem_map_size - 1].base + mem_map[mem_map_size - 1].size) / 0x1000;

	u32 i = 0;
	u32 j = 0;
	//find first useable memory region larger than bitmap_size
	for (; i < mem_map_size; i++) {
		if (mem_map[i].type == 1 && (mem_map[i].size > bitmap_size)) {
			break;
		}
	}

	// Plonk bitmap into the first free memory spot large enough
	u32 *t_mem = (u32 *)mem_map[i].base;

	// Fill the bitmap with 1s
	for (j = 0; j < bitmap_size; j++) {
		t_mem[j] = 0xFFFFFFFF;
	}

	// Clear out the large usable space found.
	for (j = 0; j < bitmap_size; j++) {
		if ((j >= (mem_map[i].base / 0x1000))  && (j < ((mem_map[i].base + mem_map[i].size) / 0x1000))) {
			t_mem[j] = 0x0;
		}
	}

	// Fill the space used by the mem structure itself
	for (j = (mem_map[i].base / 0x1000); j < (mem_map[i].base / 0x1000) + (bitmap_size / 0x1000); j++) {
		t_mem[j] = 0xFFFFFFFF;
	}

	first_hole = j;
	memory = t_mem;

	puts("initialized physical memory bitmap!");

	p_page_dir = (u32 *)alloc_p();
	for (u32 i = 0; i < 1023; i++) {
		p_page_dir[i] = new_pde();
	}


	// Make this a recursive page table, so I can poke at it from virtual addr space
	p_page_dir[1023] = ((u32)p_page_dir & ~0xFFF) | 1;

	for (u32 i = 0; i < 1024; i++) {
		//print("physical page dir entry [0x"); putn(i, 16); print("]: 0x"); putn(p_page_dir[i], 16); putc('\n');
		u32 *addr = (u32 *)p_page_dir[i];
		addr = (u32 *)new_pte();
	}
	puts("generated page directory!");
	//print("p_page_dir: 0x"); putn(p_page_dir, 16); putc('\n');

	u32 cr0;
	asm volatile("mov %0, %%cr3" :: "r"(p_page_dir));
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 1 << 31;
	asm volatile("mov %0, %%cr0":: "r"(cr0));

	//puts("initialized paging!");
}
