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
} page_info, *page_info_ptr;

u32 MEMORY_SIZE;
u32 *memory;

mem_map_ptr *mem_map;
u16 mem_map_size;

u32 *page_dir = (u32 *)0xFFFFF000;

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

void init_mem() {
	MEMORY_SIZE = (mem_map[mem_map_size - 1].base + mem_map[mem_map_size - 1].size) / 0x1000;

	u32 i = 0;
	u32 j = 0;
	//find first useable memory region larger than MEMORY_SIZE
	for (; i < mem_map_size; i++) {
		if (mem_map[i].type == 1 && (mem_map[i].size > MEMORY_SIZE)) {
			break;
		}
	}

	// Plonk bitmap into the first free memory spot large enough
	u32 *t_mem = (u32 *)mem_map[i].base;

	// Fill the bitmap with 1s
	for (j = 0; j < MEMORY_SIZE; j++) {
		t_mem[j] = 0xFFFFFFFF;
	}

	for (j = 0; j < MEMORY_SIZE; j++) {
		if ((j >= (mem_map[i].base / 0x1000))  && (j < ((mem_map[i].base + mem_map[i].size) / 0x1000))) {
			t_mem[j] = 0x0;
		}
	}

	//print_bitset(i - 1);
	//putn(i, 16);
	//print("i: 0x"); putn(i, 16); putc('\n');

	memory = t_mem;
	puts("Memory bitmap initialized!");

	puts("Checking for weird overlaps!");
	print_bitset(mem_map[i].base / 0x1000 - 1);
	print_bitset(mem_map[i].base / 0x1000);
	print_bitset((mem_map[i].base / 0x1000) + 1);
	print_bitset((mem_map[i+1].base / 0x1000) - 1);
	print_bitset((mem_map[i+1].base / 0x1000));
}

i32 alloc(u32 size) {
	if (size <= 0) {
		return -1;
	}

	i32 count = 0;
	i32 tmp_size = size;
	for (u32 i = 0; i < MEMORY_SIZE; i++) {
		if (!is_set(memory[index_of(i)], offset_from(i))) {
			count++;
		} else if (count > 0) {
			if (count >= (i32)size) {
				for (u32 j = i - count; j < i; j++) {
					if (tmp_size > 0) {
						memory[index_of(j)] = set_bit(memory[index_of(j)], offset_from(j));
						tmp_size--;
					} else {
						return i - count;
					}
				}
				if (tmp_size == 0) {
					return i - count;
				}
			}
			count = 0;
		}
	}
	if (count > 0) {
		if (count >= (i32)size) {
			for (u32 j = MEMORY_SIZE - count; j < MEMORY_SIZE; j++) {
				if (tmp_size > 0) {
					memory[index_of(j)] = set_bit(memory[index_of(j)], offset_from(j));
					tmp_size--;
				}
			}
			if (tmp_size == 0) {
				return MEMORY_SIZE - count;
			}
		}
	}

	return -1;
}

i32 free(i32 addr, u32 size) {

	if (addr < 0) {
		return -1;
	}

	for (u32 i = addr; i < (addr + size); i++) {
		memory[index_of(i)] = clear_bit(memory[index_of(i)], offset_from(i));
	}
	return 0;
}

page_info vaddr_to_page_idx(u32 addr) {
	page_info pginf;

	addr &= ~0xFFF;
	pginf.page_table = addr / 0x400000;
	pginf.page = (addr % 0x400000) / 0x1000;
	return pginf;
}

i32 map_page(u32 p_addr, u32 v_addr) {
	page_info pginf = vaddr_to_page_idx(v_addr);

	if (page_dir[pginf.page_table] & 1) {
		// table exists
		u32 *page_table = (u32 *)(0xFFC00000 + (pginf.page_table * 0x1000)); // virt addr of page table
		if (!page_table[pginf.page] & 1) {
			// page not yet mapped
			page_table[pginf.page] = p_addr | 3;
		} else {
			// page already mapped
			return -1;
		}
	} else {
		// No table yet, so allocate a page, add into page_dir
		u32 *new_page_table = (u32 *)alloc(1);
		u32 *page_table = (u32 *)(0xFFC00000 + (pginf.page_table * 0x1000));

		page_dir[pginf.page_table] = (u32)new_page_table | 3; // Throw the new page table into the page dir
		page_table[pginf.page] = p_addr | 3;
	}
	return 1;
}

void unmap_page(u32 v_addr) {
	page_info pginf = vaddr_to_page_idx(v_addr);

	if (page_dir[pginf.page_table] & 1) {
		i32 i;
		u32 *page_table = (u32 *)(0xFFC00000 + (pginf.page_table * 0x1000));
		if (page_table[pginf.page] & 1) {
			// page is mapped, unmap it
			page_table[pginf.page] = 2; // r/w, not present
		}

		// Look for more present PTEs
		for (i = 0; i < 1024; i++) {
			if (page_table[i] & 1) break;
		}

		// If none, free space allocated to the page table, removing mappings
		if (i == 1024) {
			free(page_dir[pginf.page_table] & 0xFFFFF000, 1);
			page_dir[pginf.page_table] = 2;
		}
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
