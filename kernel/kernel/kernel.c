#include <stdio.h>
#include <math.h>

#include <kernel/multiboot.h>
#include <kernel/memory_mapper.h>
#include <kernel/liballoc.h>
#include <kernel/tty.h>

#ifndef __is_x64
	#include <kernel/cpu/gdt.h>
#endif

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

void update_screen() {
	if (vga_enabled) {
		// Update the screen
		wm_draw();

		// Draw the cursor then swap buffers
		vga_drawcursor();
		vga_swapbuffers();

		mouse_update();
	}
}

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

#ifndef __is_x64
	printf("Installing GDT... ");
	gdt_install();
	printf("done\n");
#endif

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

	if (mbt->framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_RGB) {
		printf("Initialising window manager... ");
		wm_init();
		printf("done\n");
	}

	printf("Initialisation Complete!\n");

	printf("\n");
	printf("                                           _L/L\n                                          _LT/l_L_\n                                        _LLl/L_T_lL_\n                    _T/L              _LT|L/_|__L_|_L_\n                  _Ll/l_L_          _TL|_T/_L_|__T__|_l_\n                _TLl/T_l|_L_      _LL|_Tl/_|__l___L__L_|L_\n              _LT_L/L_|_L_l_L_  _'|_|_|T/_L_l__T _ l__|__|L_\n            _Tl_L|/_|__|_|__T _LlT_|_Ll/_l_ _|__[ ]__|__|_l_L_\n      __  _LT_l_l/|__|__l_T _T_L|_|_|l/___|__ | _l__|_ |__|_T_L_  __\n\n                             Pyramid OS\n\n");
	

	printf("%s> ", current_directory);

	while(1) {
		update_screen();
	}
}
