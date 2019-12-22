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

/* TODO: Expand this function to do more than just map memory based off of len */
void *mem_map(size_t len, int prot, int flags) {
	if (len <= 0) {
		return MAP_FAILED;
	}

	void* pa;

	memory_block_t* next = &map;
	while(next != 0) {
		if (next->size >= (len + sizeof(memory_block_t)) && next->free) {
			pa = next->addr;
			
			memory_block_t* n = next->addr + len;
			n->free = 1;
			n->size = next->size - sizeof(memory_block_t) - len;
			n->addr = next->addr + len + sizeof(memory_block_t);
			n->next_ptr = next->next_ptr;

			next->next_ptr = n;
			next->free = 0;
			next->size = len;
	
			return pa;
		}
		next = next->next_ptr;
	}

	(void)prot;
	(void)flags;

	return MAP_FAILED;
}

void optimise_memory_block(memory_block_t* mem) {
	if (mem->next_ptr != 0) {
		if (mem->next_ptr->free) {
			if (mem->next_ptr->addr == mem->addr + mem->size + sizeof(memory_block_t)) {
				mem->size += mem->next_ptr->size + sizeof(memory_block_t);
				mem->next_ptr = mem->next_ptr->next_ptr;
			}
		}
	}
}

/* TODO: Expand this function to take len into account */
int mem_unmap(void *addr) {
	memory_block_t* mem = &map;
	while (mem != 0) {
		if (mem->addr == addr) {
			mem->free = 1;
			optimise_memory_block(mem);
			return 0;
		}
		mem = mem->next_ptr;
	}

	return -1;
}
