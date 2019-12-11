#include <kernel/memory_mapper.h>
#include <kernel/cpu/timer.h>

#include <stdio.h>
#include <assert.h>
#include <string.h>

void memory_mapper_init(multiboot_info_t* mbt) {	
	memory_block_t* prev = 0;

	multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)mbt->mmap_addr;
	while(mmap < mbt->mmap_addr + mbt->mmap_length) {
		if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
			// The memory is available for use
			if (prev == 0) {
				map.free = 1;
				map.size = (size_t)mmap->len - sizeof(memory_block_t);
				map.addr = mmap->addr + sizeof(memory_block_t);
				map.next_ptr = 0;
				prev = &map;
			} else {
				memory_block_t* next = (memory_block_t*)(mmap->addr);
				next->free = 1;
				next->size = (size_t)mmap->len - sizeof(memory_block_t);
				next->addr = mmap->addr + sizeof(memory_block_t);
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
