#include <stdio.h>
#include <math.h>

#include <kernel/multiboot.h>
#include <kernel/memory_mapper.h>
#include <kernel/tty.h>
#include <kernel/cpu/gdt.h>
#include <kernel/cpu/idt.h>
#include <kernel/cpu/timer.h>

#include <kernel/driver/ata.h>
#include <kernel/driver/vfs.h>
#include <kernel/driver/keyboard.h>

void kernel_main(multiboot_info_t* mbt, unsigned int magic) {
	terminal_initialize();
	printf("Hello, kernel World!\nCreated By Millie!\n");
	printf("Test Number: %f\n", sin(3.14159265/4));
	printf("Lower memory: %fmb\n", (double)mbt->mem_lower / 1024.0);
	printf("Upper memory: %fmb\n", (double)mbt->mem_upper / 1024.0);

	gdt_install();
	idt_install();
	memory_mapper_init(mbt);
	ide_init(0x1F0, 0x3F6, 0x170, 0x376, 0x000);
	vfs_init();
	timer_init(1000);

	printf("%s> ", currentPath);

	while(1) {}
}
