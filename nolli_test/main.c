#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 64

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef enum {false = 0, true = 1} bool;

u32 memory[2] = {0x7314877, 0xE7314877};

u32 set_bit(u32 map, u8 nbit) {
	return map | (1 << nbit);
}

u32 clear_bit(u32 map, u8 nbit) {
	return map ^ (1 << nbit);
}

bool is_set(u32 map, u8 nbit) {
	return !!(map & (1 << nbit));
}

int index_of(u32 i) {
	return i / (sizeof(u32) * 8);
}

int offset_from(u32 i) {
	return i % (sizeof(u32) * 8);
}

void print_memory() {
	int tmp = 0;
	for (int i = 0; i < MEMORY_SIZE; i++) {
		int idx = index_of(i);
		/*if (idx > tmp) { //Wraps line for greater readability
			puts("");
			tmp = idx;
		}*/
		printf("%d",is_set(memory[idx], offset_from(i)));
	}
}

int alloc(int size) {
	printf("alloc: %d\n", size);
	if (size <= 0) {
		puts("You can't allocate that!");
		return -1;
	}
	int count = 0;
	int tmp_size = size;
	for (int i = 0; i < MEMORY_SIZE; i++) {
		if (!is_set(memory[index_of(i)], offset_from(i))) {
			count++;
		} else if (count > 0) {
			if (count >= size) {
				for (int j = i - count; j < i; j++) {
					if (tmp_size > 0) {
						memory[index_of(j)] = set_bit(memory[index_of(j)], offset_from(j));
						tmp_size--;
					} else {
						printf("returned address: %d\n", i - count);
						return i - count;
					}
				}
				if (tmp_size == 0) {
					printf("returned address: %d\n", i - count);
					return i - count;
				}
			}
			count = 0;
		}
	}
	if (count > 0) {
		if (count >= size) {
			for (int j = MEMORY_SIZE - count; j < MEMORY_SIZE; j++) {
				if (tmp_size > 0) {
					memory[index_of(j)] = set_bit(memory[index_of(j)], offset_from(j));
					tmp_size--;
				}
			}
			if (tmp_size == 0) {
				printf("returned address: %d\n", MEMORY_SIZE - count);
				return MEMORY_SIZE - count;
			}
		}
	}


	printf("Ur BORKED, probably allocated too much memory!\n");
	return -1;
}

void free_m(int addr, int size) {
	printf("free: %d\n", size);

	if (addr < 0) {
		puts("Can't free null!");
		return;
	}

	int count = 0;
	for (int i = addr; i < (addr + size); i++) {
		memory[index_of(i)] = clear_bit(memory[index_of(i)], offset_from(i));
	}
}

int main() {
	printf("[TEST-PLATFORM] I'm not in the kernel!!!\n");
	printf("Testing first fit allocator!\n\n");

	puts("initial state");
	print_memory();
	puts("");

	int pen = alloc(2);
	print_memory();
	puts("");

	int bike = alloc(2);
	print_memory();
	puts("");

	int lol = alloc(5);
	print_memory();
	puts("");

	int zip = alloc(1);
	print_memory();
	puts("");

	int roop = alloc(1);
	print_memory();
	puts("");

	free_m(pen, 2);
	print_memory();
	puts("");

	free_m(bike, 2);
	print_memory();
	puts("");

	free_m(lol, 5);
	print_memory();
	puts("");

	free_m(zip, 1);
	print_memory();
	puts("");

	free_m(roop, 1);
	print_memory();
	puts("");

	return 0;
}
