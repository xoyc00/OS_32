#include <stdio.h>
#include <math.h>

#include <kernel/multiboot.h>
#include <kernel/tty.h>
#include <kernel/cpu/gdt.h>
#include <kernel/cpu/idt.h>

#include <kernel/drivers/keyboard.h>

void kernel_main(multiboot_info_t* mbd, unsigned int magic) {
	terminal_initialize();
	printf("Hello, kernel World!\nCreated By Millie!\n");
	printf("Test Number: %f\n", sin(3.14159265/4));
	printf("Lower memory: %fmb\n", (double)mbd->mem_lower / 1024.0);
	printf("Upper memory: %fmb\n", (double)mbd->mem_upper / 1024.0);

	gdt_install();
	idt_install();
	

	printf("> ");

	while(1) {}
}
