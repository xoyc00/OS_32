#include <stdio.h>

#if defined(__is_libk)
#include <kernel/tty.h>
#include <kernel/driver/vga/vga.h>

extern int vga_drvr_enabled;
#endif

int putchar(int ic) {
#if defined(__is_libk)
	char c = (char) ic;
	if (vga_drvr_enabled) {
		vga_terminal_write(&c, sizeof(c));
	} else {
		terminal_write(&c, sizeof(c));
	}
#else
	asm ("mov $0b000, %%eax" ::: "eax");
	asm ("mov %0, %%ebx" :: "r"(ic) : "ebx");
	asm volatile ("int $0x80");
#endif
	return ic;
}
