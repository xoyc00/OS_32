#ifndef _MEMORY_MAPPER_H
#define _MEMORY_MAPPER_H
#include <kernel/multiboot.h>
#include <stdint.h>
#include <stddef.h>

typedef struct memory_block {
	struct memory_block* next_ptr;
	size_t size;
	size_t addr;
	int free;
} memory_block_t;

memory_block_t map;

void* kmalloc(size_t size);
void kfree(void* ptr);

void memory_mapper_init(multiboot_info_t* mbt);

void print_memory_map();

#endif
