#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE 0x1000
#define MAX_SIZE 512

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef enum {false = 0, true = 1} bool;

u8 *memory;
u8 bitmask[8];

typedef struct node {
	struct node *left;
	struct node *right;
	bool filled;
} node_t;

node_t *buddy_head;

node_t *list_init() {
	node_t *head = (node_t *)malloc(sizeof(node_t));
	head->left = NULL;
	head->right = NULL;
	head->filled = false;
	return head;
}

u32 mark_bit(u32 entry, u8 nbit) {
	u32 result = entry | (1 << nbit);
	return result;
}

u32 clear_bit(u32 entry, u8 nbit) {
	u32 result = entry ^ (1 << nbit);
	return result;
}

// index_in_level_of(p, n) == (p - _buffer_start) / size_of_level(n)
// total_size == (num_of_blocks(MAX_LEVELS) * sizeof(leaf))
// size_of_level(n) == total_size / (1 << n)
// max_blocks_of_level(n) == (1 << n)


void print_binary(u32 number) {
	for (u32 i = 1 << 31; i > 0; i = i / 2)
		(number & i)? putc('1', stdout): putc('0', stdout);
}

void print_addr(u32 addr) {
	putc('[', stdout);
	print_binary(memory[addr]);
	putc(']', stdout);
}

void print_memory() {
	for (int i = 0; i < MEMORY_SIZE; i++) {
		print_addr(i);
	}
	puts("");
}

void alloc(int size) {
	if (size <= 64) {
		puts("allocating smallest size");

		for (int i = 0; i < size; i++) {
			memory[i] = 1;
		}
	} else {
		puts("spreading across multiple pages");
	}
}

void test_bit_manipulation() {
	printf("mark bit test: ");
	print_binary(mark_bit(8, 1));
	printf(" == ");
	print_binary(10);
	puts("");
	assert(mark_bit(8, 1) == 10);

	printf("clear bit test: ");
	print_binary(clear_bit(0xFFFFFFFF, 1));
	printf(" == ");
	print_binary(0xFFFFFFFD);
	puts("");
	assert(clear_bit(0xFFFFFFFF, 1) == 0xFFFFFFFD);
}

int main() {
	printf("[TEST-PLATFORM] I'm not in the kernel!!!\n");
	printf("Testing buddy allocator!\n");

	memory = (u8 *)malloc(MEMORY_SIZE);

	//alloc(64);
	//print_memory();

	free(memory);
	return 0;
}
