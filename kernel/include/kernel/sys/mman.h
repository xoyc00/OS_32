#ifndef _MMAN_H
#define _MMAN_H

#include <kernel/sys/types.h>
#include <stddef.h>

#define PROT_NONE 	0
#define PROT_EXEC 	1
#define PROT_READ 	1<<1
#define PROT_WRITE 	1<<2

#define MAP_FIXED 	1
#define MAP_PRIVATE 1<<1
#define MAP_SHARED 	1<<2

#define MAP_FAILED (void*)0

void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);
int munmap(void *addr, size_t len);

#endif
