#include <stdlib.h>

#if defined(__is_libk)
#include <kernel/memory_mapper.h>
#endif

void free(void* ptr) {
#if defined(__is_libk)
	return free(ptr);
#else
	return 0;		// TODO: Implement syscall
#endif
}
