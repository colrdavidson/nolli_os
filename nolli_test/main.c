#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 8

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef enum {false = 0, true = 1} bool;

u8 memory = 0b00110100;

u32 set_bit(u32 map, u8 nbit) {
	return map | (1 << nbit);
}

void clear_bit(u32 map, u8 nbit) {
	map = map ^ (1 << nbit);
}

bool is_set(u32 map, u8 nbit) {
	return !!(map & (1 << nbit));
}

// index_in_level_of(p, n) == (p - _buffer_start) / size_of_level(n)
// total_size == (num_of_blocks(MAX_LEVELS) * sizeof(leaf))
// size_of_level(n) == total_size / (1 << n)
// max_blocks_of_level(n) == (1 << n)

void print_addr(u8 addr) {
	printf("[%d]", is_set(memory, addr));
}

void print_memory() {
	for (int i = 0; i < MEMORY_SIZE; i++) {
		print_addr(i);
	}
	puts("");
}

void alloc(int size) {
	printf("alloc: %d\n", size);
	int count = 0;
	int tmp_size = size;
	for (int i = 0; i < MEMORY_SIZE; i++) {
		if (!is_set(memory, i)) {
			count++;
		} else if (count > 0) {
			if (count >= size) {
				for (int j = i - count; j < i; j++) {
					if (tmp_size > 0) {
						memory = set_bit(memory, j);
						tmp_size--;
					} else {
						return;
					}
				}
			}
			count = 0;
		}
	}
	if (count > 0) {
		if (count >= size) {
			for (int j = MEMORY_SIZE - count; j < MEMORY_SIZE; j++) {
				if (tmp_size > 0) {
					memory = set_bit(memory, j);
					tmp_size--;
				} else {
					return;
				}
			}
		}
	}
	if (tmp_size == 0) {
		return;
	}

	printf("Ur BORKED, probably allocated too much memory!\n");
}

void free_m(int addr, int size) {
	printf("free: %d\n", size);

	if (addr < 0) {
		puts("Can't free null!");
		return;
	}

	int count = 0;
	for (int i = addr; i < (addr + size); i++) {
		clear_bit(memory, i);
	}
}

int main() {
	printf("[TEST-PLATFORM] I'm not in the kernel!!!\n");
	printf("Testing first fit allocator!\n\n");

	puts("initial state");
	print_memory();
	puts("");

	alloc(2);
	print_memory();
	puts("");

	alloc(2);
	print_memory();
	puts("");

	alloc(5);
	print_memory();
	puts("");

	alloc(1);
	print_memory();
	puts("");

	alloc(1);
	print_memory();
	puts("");

	/*free_m(pen, 3);
	print_memory();
	puts("");

	free_m(bike, 2);
	print_memory();
	puts("");

	free_m(lol, 5);
	print_memory();
	puts("");*/

	printf("01110110, bit 0: %d\n", is_set(118, 0));
	printf("01110110, bit 1: %d\n", is_set(118, 1));
	printf("01110110, bit 2: %d\n", is_set(118, 2));
	printf("01110110, bit 3: %d\n", is_set(118, 3));
	printf("01110110, bit 4: %d\n", is_set(118, 4));
	printf("01110110, bit 5: %d\n", is_set(118, 5));
	printf("01110110, bit 6: %d\n", is_set(118, 6));
	printf("01110110, bit 7: %d\n", is_set(118, 7));

	return 0;
}
