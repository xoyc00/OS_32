#include <stdio.h>
#include <math.h>

#include <kernel/multiboot.h>
#include <kernel/liballoc.h>
#include <kernel/tty.h>
#include <kernel/cpu/gdt.h>
#include <kernel/cpu/idt.h>
#include <kernel/cpu/timer.h>
#include <kernel/cpu/ports.h>

#include <kernel/driver/vga/vga.h>
#include <kernel/driver/keyboard.h>
#include <kernel/driver/mouse.h>
#include <kernel/driver/ata.h>
#include <kernel/driver/fat32.h>

#include <kernel/system/window_manage.h>

#include <assert.h>
#include <stdlib.h>

int vga_enabled = 0;
size_t vga_width = 0;
size_t vga_height = 0;
size_t vga_pitch = 0;
size_t vga_bpp = 0;
size_t vga_addr = 0;

void kernel_main(multiboot_info_t* mbt, unsigned int magic) {
	if (mbt->framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_RGB) {
		// Initialise VGA Device
		vga_addr = mbt->framebuffer_addr;
		vga_pitch = mbt->framebuffer_pitch;
		vga_width = mbt->framebuffer_width;
		vga_height = mbt->framebuffer_height;
		vga_bpp = mbt->framebuffer_bpp;
		vga_enabled = 1;
		vga_init(vga_width, vga_height, vga_pitch, vga_bpp, vga_addr);
		mouse_init();
	} else {
		terminal_initialize();
	}
	printf("Hello, kernel World!\nCreated By Millie! %c\n", 0x02);
	printf("Test Number: %f\n", sin(3.14159265/4));

	printf("Installing GDT... ");
	gdt_install();
	printf("done\n");

	printf("Installing IDT... ");
	idt_install();
	printf("done\n");

	printf("Initialising memory mapper... ");
	memory_mapper_init(mbt);
	printf("done\n");

	printf("Initialising timer... ");
	timer_init(100000);
	printf("done\n");

	printf("Initialising ATA... ");
	ata_initialise(0x1F0, 0x3F6, 0x170, 0x376, 0x000);
	printf("done\n");
	
	ata_list_devices();

	printf("Initialising FAT32... ");
	fat32_init(0);
	printf("done\n");

	printf("Initialising window manager... ");
	wm_init();
	printf("done\n");

	printf("Initialisation Complete!\n");

	printf("%s> ", current_directory);

	while(1) {
		if (vga_enabled) {
			// Update the screen
			vga_clearscreen(255, 255, 255);
			vga_terminal_draw();

			wm_draw();

			// Draw the cursor then swap buffers
			vga_drawcursor();
			vga_swapbuffers();

			mouse_update();
		}
	}
}
