#include <stdio.h>
#include <math.h>

#include <kernel/multiboot.h>
#include <kernel/memory_mapper.h>
#include <kernel/tty.h>
#include <kernel/cpu/gdt.h>
#include <kernel/cpu/idt.h>

#include <kernel/drivers/keyboard.h>

void kernel_main(multiboot_info_t* mbt, unsigned int magic) {
	terminal_initialize();
	printf("Hello, kernel World!\nCreated By Millie!\n");
	printf("Test Number: %f\n", sin(3.14159265/4));
	printf("Lower memory: %fmb\n", (double)mbt->mem_lower / 1024.0);
	printf("Upper memory: %fmb\n", (double)mbt->mem_upper / 1024.0);

	gdt_install();
	idt_install();
	memory_mapper_init(mbt);

	kmalloc(1);
	kmalloc(10);
	kmalloc(256);
	void* ptr = kmalloc(256);
	kfree(ptr);
	kmalloc(128);
	kmalloc(128);

	printf("> ");

	while(1) {}
}
