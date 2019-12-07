#include <kernel/memory_mapper.h>

#include <stdio.h>
#include <assert.h>

void* get_next_block_of_size(size_t size) {
	if (map.size >= size + sizeof(memory_block_t)  && map.free && map.size > 0 && map.addr > 0) {
		memory_block_t* next = (memory_block_t*)(map.addr + size);
		next->size = (map.size - size) - sizeof(memory_block_t);
		next->addr = (size_t)next + sizeof(memory_block_t);
		next->free = 1;
		next->next_ptr = map.next_ptr;

		map.free = 0;
		map.size = size;
		map.next_ptr = next;
		return (void*)map.addr;
	} else {
		memory_block_t* next = map.next_ptr;
		while(next != 0) {
			if (next->size >= size + sizeof(memory_block_t) && next->free && next->size > 0 && next->addr > 0) {
				memory_block_t* n = (memory_block_t*)(next->addr + size);
				n->size = (next->size - size) - sizeof(memory_block_t);
				n->addr = (size_t)n + sizeof(memory_block_t);
				n->free = 1;
				n->next_ptr = next->next_ptr;

				next->free = 0;
				next->size = size;
				next->next_ptr = n;
				return (void*)next->addr;
			} else {
				next = next->next_ptr;
			}
		}
	}

	return 0;
}

void optimise_blocks(memory_block_t* m) {
	memory_block_t* n = m->next_ptr;
	if (m->free && n->free && n->addr == (m->addr + sizeof(memory_block_t) + m->size)) {
		m->next_ptr = n->next_ptr;
		m->size += n->size;
		m->size += sizeof(memory_block_t);
	}
}

void* kmalloc(size_t size) {
	return get_next_block_of_size(size);
}

void kfree(void* ptr) {
	memory_block_t* to_free = &map;
	
	while (to_free) {
		if (to_free->addr == ptr)
			break;
		else
			to_free = to_free->next_ptr;
	}

	if (to_free) {
		to_free->free = 1;
		optimise_blocks(to_free);
	}
}

void memory_mapper_init(multiboot_info_t* mbt) {	
	memory_block_t* prev = 0;
	int counter = 0;

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
			counter ++;
		}		
	
		mmap = (multiboot_memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(mmap->size) );
	}

	printf("Found %i free memory blocks.\n", counter);
}

void print_memory_map() {
	memory_block_t* next = &map;
	while(next != 0) {
		if (next->size > 0 && next->addr > 0)
			printf("\t Next:\n\t\t Free: %i\n\t\t Size: %fMB\n\t\t Address: %i\n\t\t Next_ptr: %i\n", next->free, (double)next->size / 1024 / 1024, next->addr, next->next_ptr);
		next = next->next_ptr;
	}
}
