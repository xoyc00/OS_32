#include <stdio.h>
#include <assert.h>

#include <kernel/tty.h>
#include <kernel/cpu/gdt.h>
#include <kernel/cpu/idt.h>

#include <kernel/drivers/keyboard.h>

void kernel_main(void) {
	terminal_initialize();
	printf("Hello, kernel World!\nCreated By Millie!\n");
	printf("Test Number: %x\n", 27);

	gdt_install();
	idt_init();

	unsigned int i;
	while(1) {
		if (i % 100000000 == 0)
			printf("\nHello!");

		i ++;
	}
}
