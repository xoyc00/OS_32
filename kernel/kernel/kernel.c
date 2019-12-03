#include <stdio.h>
#include <math.h>

#include <kernel/tty.h>
#include <kernel/cpu/gdt.h>
#include <kernel/cpu/idt.h>

#include <kernel/drivers/keyboard.h>

void kernel_main(void) {
	terminal_initialize();
	printf("Hello, kernel World!\nCreated By Millie!\n");
	printf("Test Number: %f\n", tan(3.14159265/4));

	gdt_install();
	idt_init();

	while(1) {}
}
