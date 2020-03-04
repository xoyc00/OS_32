#include <stdlib.h>

#if defined(__is_libk)
#include <kernel/liballoc.h>
#endif

void free(void* ptr) {
#if defined(__is_libk)
	return kfree(ptr);
#else
	asm ("mov $0b010, %%eax" ::: "eax");
	asm ("mov %0, %%ebx" :: "r"(ptr) : "ebx");
	asm volatile ("int $0x80");
#endif
}
