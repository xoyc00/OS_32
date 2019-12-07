#include <stdio.h>
#include <math.h>

#include <kernel/multiboot.h>
#include <kernel/memory_mapper.h>
#include <kernel/tty.h>
#include <kernel/cpu/gdt.h>
#include <kernel/cpu/idt.h>
#include <kernel/cpu/timer.h>
#include <kernel/cpu/ports.h>

#include <kernel/driver/vga/vga.h>
#include <kernel/driver/keyboard.h>
#include <kernel/driver/mouse.h>

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
	printf("Hello, kernel World!\nCreated By Millie!\n");
	printf("Test Number: %f\n", sin(3.14159265/4));
	printf("Lower memory: %fmb\n", (double)mbt->mem_lower / 1024.0);
	printf("Upper memory: %fmb\n", (double)mbt->mem_upper / 1024.0);

	gdt_install();
	idt_install();
	memory_mapper_init(mbt);
	timer_init(100000);

	printf("> ");

	double fps;

	while(1) {
		if (vga_enabled) {
			uint32_t start = timer_get_ticks();

			// Update the screen
			vga_clearscreen(48, 48, 48);
			vga_terminal_draw();

			char* fpsstr;
			ftoa(fps, fpsstr, 2);
			vga_drawstr("FPS:", 1184, 0, 255, 255, 255);
			vga_drawstr(fpsstr, 1216, 0, 255, 255, 255);

			vga_drawcursor();

			// Swap buffers
			vga_swapbuffers();

			uint32_t end = timer_get_ticks();

			fps = 100000.0 / (double)((end - start));
		}
	}
}
