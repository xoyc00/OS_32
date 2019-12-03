#include <kernel/memory_mapper.h>

#include <stdio.h>

void* get_next_block_of_size(size_t size) {
	if (map.size >= size && map.free) {
		memory_block_t* next = (memory_block_t*)(map.addr + size);
		next->size = (map.size - size) - sizeof(memory_block_t);
		next->addr = (size_t)next + sizeof(memory_block_t);
		next->free = 1;
		next->next_ptr = map.next_ptr;

		map.free = 0;
		map.size = size;
		map.next_ptr = next;
		printf("Map:\n Size: %i\n Addr: 0x%x\n", map.size, map.addr);
		return (void*)map.addr;
	} else {
		memory_block_t* next = map.next_ptr;
		while(next != 0) {
			if (next->size >= size && next->free) {
				memory_block_t* n = (memory_block_t*)(next->addr + size);
				n->size = (next->size - size) - sizeof(memory_block_t);
				n->addr = (size_t)n + sizeof(memory_block_t);
				n->free = 1;
				n->next_ptr = next->next_ptr;

				next->free = 0;
				next->size = size;
				next->next_ptr = n;
				printf("Next:\n Size: %i\n Addr: 0x%x\n", next->size, next->addr);
				return (void*)next->addr;
			} else {
				next = next->next_ptr;
			}
		}
	}

	return 0;
}

void optimise_blocks() {
	memory_block_t* next = &map;
	while(next != 0) {
		memory_block_t* n = (memory_block_t*)(next->addr + next->size);
		if (next->free && n->free) {
			next->next_ptr = n->next_ptr;
			next->size += n->size;
			next->size += sizeof(memory_block_t);
		}

		next = next->next_ptr;
	}
}

void* kmalloc(size_t size) {
	void* out_ptr = get_next_block_of_size(size);	
	if (out_ptr == 0) {
		return 0;
	}

	return out_ptr;
}

void kfree(void* ptr) {
	memory_block_t* to_free = (memory_block_t*)(ptr - sizeof(memory_block_t));
	to_free->free = 1;
	optimise_blocks();
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
				map.size = (size_t)mmap->size - sizeof(memory_block_t);
				map.addr = mmap->addr + sizeof(memory_block_t);
				map.next_ptr = 0;
				prev = &map;
			} else {
				memory_block_t* next = (memory_block_t*)mmap->addr - sizeof(memory_block_t);
				next->free = 1;
				next->size = (size_t)mmap->size - sizeof(memory_block_t);
				next->addr = mmap->addr;
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
