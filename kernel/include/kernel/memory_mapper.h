#ifndef _MEMORY_MAPPER_H
#define _MEMORY_MAPPER_H
#include <kernel/multiboot.h>
#include <stdint.h>
#include <stddef.h>

/* Holds information about a given block of memory */
typedef struct memory_block {
	struct memory_block* next_ptr;
	size_t size;
	size_t addr;
	int free;
} memory_block_t;

/* Hold the root memory block of the linked list */
memory_block_t map;

/* Initialises the memory mapper */
void memory_mapper_init(multiboot_info_t* mbt);

/* Print the memory map to the terminal */
void print_memory_map();

#endif
