#include <stdio.h>
#include <assert.h>

#include <kernel/tty.h>
#include <kernel/cpu/gdt.h>

void kernel_main(void) {
	terminal_initialize();
	printf("Hello, kernel World!\nCreated By Millie!\n");
	printf("Test Number: %x\n", 27);

	gdt_install();

	assert(0);
}
