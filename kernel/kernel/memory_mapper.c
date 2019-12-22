#include <kernel/memory_mapper.h>
#include <kernel/cpu/timer.h>

#include <stdio.h>
#include <assert.h>
#include <string.h>

extern void* endkernel;

void memory_mapper_init(multiboot_info_t* mbt) {	
	memory_block_t* prev = 0;

	multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)mbt->mmap_addr;
	while(mmap < mbt->mmap_addr + mbt->mmap_length) {
		if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
			// The memory is available for use
			if (prev == 0 && mmap->addr > endkernel) {
				map.free = 1;
				map.size = (size_t)mmap->len;
				map.addr = mmap->addr;
				map.next_ptr = 0;
				prev = &map;
			} else if (prev == 0 && mmap->addr + mmap->len > endkernel)  {
				map.free = 1;
				map.size = (size_t)mmap->len - (size_t)(mmap->addr - (size_t)endkernel);
				map.addr = endkernel;
				map.next_ptr = 0;
				prev = &map;
			} else if (mmap->addr > endkernel) {
				memory_block_t* next = (memory_block_t*)(mmap->addr);
				next->free = 1;
				next->size = (size_t)mmap->len - sizeof(memory_block_t);
				next->addr = mmap->addr + sizeof(memory_block_t);
				next->next_ptr = 0;
				prev->next_ptr = next;
				prev = next;
			} else if (mmap->addr + mmap->len > endkernel)  {
				memory_block_t* next = (memory_block_t*)(endkernel);
				next->free = 1;
				next->size = (size_t)mmap->len - sizeof(memory_block_t) - (size_t)(mmap->addr - (size_t)endkernel);
				next->addr = endkernel + sizeof(memory_block_t);
				next->next_ptr = 0;
				prev->next_ptr = next;
				prev = next;
			}
		}		
	
		mmap = (multiboot_memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(mmap->size) );
	}
}

void print_memory_map() {
	memory_block_t* next = &map;
	while(next != 0) {
		printf("\t Next:\n\t\t Free: %i\n\t\t Size: %fMB\n\t\t Address: %i\n\t\t Next_ptr: %i\n", next->free, (double)next->size / 1024 / 1024, next->addr, next->next_ptr);
		next = next->next_ptr;
	}
}
