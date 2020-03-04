#include <stdlib.h>
#include <stdint.h>

#if defined(__is_libk)
#include <kernel/liballoc.h>
#endif

void* malloc(size_t size) {
#if defined(__is_libk)
	return kmalloc(size);
#else
	asm ("mov $0b001, %%eax" ::: "eax");
	asm ("mov %0, %%ebx" :: "r"(size) : "ebx");
	asm volatile ("int $0x80");
	void* out;
	asm ("mov %%eax, %0":"=r"(out));
	return out;
#endif
}
