#include <kernel/shell.h>

#include <kernel/memory_mapper.h>
#include <kernel/tty.h>

#include <kernel/driver/pcspkr.h>
#include <kernel/driver/vga/vga.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern int vga_drvr_enabled;

void process_input(char* input) {
	char* i1 = strtok(input, " ");
	char* iargs[256];
	for (int i = 0; i < 256; i++) {
		char* arg = strtok(0, " ");
		if (arg != 0) {
			iargs[i] = arg;
		} else {
			iargs[i] = 0;
		}
	}

    if (strcmp(i1, "shutdown") == 0) {
        printf("Stopping the CPU. It is now safe to turn off your computer.\n");
		if (vga_drvr_enabled) {
			vga_terminal_draw();

			vga_drawcursor();

			// Swap buffers
			vga_swapbuffers();
		}
        asm volatile("hlt");
    } else if (strcmp (i1, "clear") == 0) {
		if (vga_drvr_enabled) {
			vga_terminal_clear();
		} else {
			terminal_clear();
		}
	} else if (strcmp(i1, "memmap") == 0) {
		print_memory_map();
	} else if (strcmp(i1, "print") == 0) {
		for (int i = 0; i < 256; i ++) {
			if (iargs[i] != 0) {
				printf(iargs[i]);
				printf(" ");
			}
		}
		printf("\n");
	} else if (strcmp(i1, "beep") == 0) {
		pcspkr_beep();
	} else {
		printf("Not a known command or program!\n");
	}

    printf("> ");
}
