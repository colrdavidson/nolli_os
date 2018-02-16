#ifndef _MEM_H
#define _MEM_H

#include "common.h"
#include "vga.h"

typedef struct __attribute__((packed)) {
	u32 base;
	u32 base_hi;
	u32 size;
	u32 size_hi;
	u32 type;
} mem_map_ptr;

u32 bitmap_size;
u32 memory_size;
u32 *memory;

mem_map_ptr *mem_map;
u16 mem_map_size;

u32 *page_dir;
u32 first_hole;

extern void *_kernel_start;
extern void *_kernel_end;

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

u32 page_align(u32 i) {
	return i + 0x1000 - (i % 0x1000);
}

void print_bitset(u32 idx) {
	printf("set %p: %b\n", idx, memory[idx]);
}

void *alloc_p() {
	for (u32 i = first_hole; i < bitmap_size; i++) {
		if (!is_set(memory[index_of(i)], offset_from(i))) {
			memory[index_of(i)] = set_bit(memory[index_of(i)], offset_from(i));
			first_hole = i;
			return (void *)(i * 0x1000);
		}
	}

	return NULL;
}

void *alloc_pages(u32 pages) {
	u32 page_count = 0;
	u32 run_start = 0;

	for (u32 i = first_hole; i < bitmap_size; i++) {
		if (page_count == 0) {
			if (!is_set(memory[index_of(i)], offset_from(i))) {
				page_count++;
				run_start = i;
			}
		} else {
			if (!is_set(memory[index_of(i)], offset_from(i))) {
				page_count++;
			} else {
				page_count = 0;
			}
		}

		if (page_count == pages) {
			first_hole = run_start;
			for (u32 j = run_start; j <= i; j++) {
				memory[index_of(j)] = set_bit(memory[index_of(j)], offset_from(j));
			}

			return (void *)(run_start * 0x1000);
		}
	}

	return NULL;
}

void *kmalloc(u32 size) {
	u32 pages = (size / 0x1000) + 1;
	void *addr = alloc_pages(pages);
	//printf("addr: %p\n", addr);
	return addr;
}

void free(void *addr, u32 size) {
	u32 iaddr = (u32)addr / 0x1000;
	for (u32 i = iaddr; i < (iaddr + size); i++) {
		memory[index_of(i)] = clear_bit(memory[index_of(i)], offset_from(i));
	}
}

void print_map() {
	for (u32 entry = 0; entry < mem_map_size; entry++) {
		printf("mem_map[%d] range: %p - %p\n", entry, mem_map[entry].base, mem_map[entry].base + mem_map[entry].size);

		u32 type = mem_map[entry].type;
		if (type != 1) {
			printf("mem_map[%d] type: Reserved\n", entry);
		} else {
			printf("mem_map[%d] type: Usable\n", entry);
		}
	}
}

u32 new_pde() {
	u32 addr = (u32)alloc_p();
	u32 pde = (addr & ~0xFFF) | 3; // Chop off the bottom 12 bits of the address, and then set as present and r/w
	return pde;
}

void map_page(u32 *cur_page_dir, u32 v_addr, u32 p_addr) {
	u32 pd_idx = v_addr >> 22;
	u32 pt_idx = (v_addr >> 12) & 0x3FF;
	explode_if(((v_addr << 20) >> 20) != 0);

	u32 *page_table = (u32 *)((cur_page_dir[pd_idx] >> 2) << 2);
	page_table[pt_idx] = (p_addr & ~0xFFF) | 3;
}

void unmap_page(u32 *cur_page_dir, u32 v_addr) {
	u32 pd_idx = v_addr >> 22;
	u32 pt_idx = (v_addr >> 12) & 0x3FF;
	explode_if(((v_addr << 20) >> 20) != 0);

	u32 *page_table = (u32 *)((cur_page_dir[pd_idx] >> 2) << 2);
	printf("[%p] = %p\n", pt_idx, page_table[pt_idx]);
	page_table[pt_idx] = 2;
}

void init_mem() {
	u32 kstart = (u32)&_kernel_start;
	u32 kend = (u32)&_kernel_end;
	u32 ksize = kend - kstart;

	memory_size = (mem_map[mem_map_size - 1].base + mem_map[mem_map_size - 1].size);
	bitmap_size = memory_size / 0x1000;

	u32 i = 1;
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

	page_dir = (u32 *)alloc_p();
	for (u32 t = 0; t < 1023; t++) {
		page_dir[t] = new_pde();
	}

	// Make this a recursive page table, so I can poke at it from virtual addr space
	page_dir[1023] = ((u32)page_dir & ~0xFFF) | 1;

	// Identity map the everything
	for (i = 0; i < (memory_size / 0x1000); i++) {
		u32 addr = i * 0x1000;
		map_page(page_dir, addr, addr);
	}

	/*
	// Remap the kernel to the higher half
	i = page_align(kstart);
	j = page_align(0x7FE0000 - ksize);

	while (i < kend) {
		unmap_page(page_dir, i);
		map_page(page_dir, j, i);
		i += 0x1000;
		j += 0x1000;
	}

	u32 k_page_s = kstart / 0x1000;
	u32 k_page_e = kend / 0x1000;
	printf("%d -> %d\n", k_page_s, k_page_e);
	for (i = k_page_s; i < k_page_e; i++) {
		printf("page_dir[%p]: %p\n", i, page_dir[i]);
		u32 *page_table = (u32 *)((page_dir[i] >> 2) << 2);
		for (j = 0; j < 1; j++) {
			printf("    page_tab[%p]: %p\n", j, page_table[j]);
		}
	}

	kstart = 0x7FE0000;
	kend = kstart + ksize;

	*/

	/*
	// Move ACPI to 1 MB below the kernel
	i = 0;
	j = page_align(kstart - 0xFFFFF); // Kernel space - 1 MB
	while (i < 0x3E8000) {
		map_page(page_dir, j, i);
		unmap_page(page_dir, i);
		i += 0x1000;
		j += 0x1000;
	}
	*/

	// Enable paging
	u32 cr0;
	asm volatile("mov %0, %%cr3" :: "r"(page_dir));
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 1 << 31;
	asm volatile("mov %0, %%cr0":: "r"(cr0));

	puts("Set up paging and remapped the kernel!\n");
}

void flush_page_tables(void) {
	asm volatile (
		"movl %%cr3, %%eax\n"
		"movl %%eax, %%cr3\n"
		::: "%eax"
	);
}

#endif
