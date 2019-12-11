#include <stdlib.h>

#if defined(__is_libk)
#include <kernel/liballoc.h>
#endif

void free(void* ptr) {
#if defined(__is_libk)
	return kfree(ptr);
#else
	return 0;		// TODO: Implement syscall
#endif
}
