#include <kernel/sys/mman.h>
#include <kernel/memory_mapper.h>

/* TODO: Expand this function to do more than just map memory based off of len */
void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off) {
	if (len <= 0) {
		return MAP_FAILED;
	}

	void* pa;

	memory_block_t* next = &map;
	while(next != 0) {
		if (next->size >= (len + sizeof(memory_block_t))) {
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

	(void)addr;
	(void)prot;
	(void)flags;
	(void)fildes;
	(void)off;

	return MAP_FAILED;
}

/* TODO: Expand this function to take len into account */
int munmap(void *addr, size_t len) {
	memory_block_t* mem = &map;
	while (mem != 0) {
		if (mem->addr == addr) {
			mem->free = 1;
			return 0;
		}
		mem = mem->next_ptr;
	}

	(void)len;

	return -1;
}
