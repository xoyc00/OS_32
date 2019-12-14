#include <kernel/shell.h>

#include <kernel/memory_mapper.h>
#include <kernel/tty.h>

#include <kernel/driver/pcspkr.h>
#include <kernel/driver/vga/vga.h>
#include <kernel/driver/ata.h>
#include <kernel/driver/fat32.h>

#include <kernel/system/window_manage.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

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

		{
			vga_clearscreen(0, 0, 0);
			vga_terminal_draw();
	
			wm_draw();

			vga_drawcursor();
			vga_swapbuffers();
		}

        asm volatile("hlt");
    } else if (strcmp (i1, "clear") == 0) {
		if (vga_drvr_enabled) {
			vga_terminal_clear();
		} else {
			terminal_clear();
		}
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
	} else if (strcmp(i1, "drvls") == 0) {
		ata_list_devices();
	} else if (strcmp(i1, "ls") == 0) {
		int count;
		directory_entry_t* d = read_directory_from_name(0, current_directory, &count);
		if (count != 0) {
			list_directory(0, 0, count, d, 0, 1);
			free(d);
		}
	} else if (strcmp(i1, "cd") == 0) {
		char* path = iargs[0];

		if (path[0] != '/') {
			printf("Not a valid path!\n");
		} else {
			free(current_directory);

			current_directory = malloc(strlen(path)+1);
			strcpy(current_directory, path);
			current_directory[strlen(path)] = '\0';
		}
	} else if (strcmp(i1,"dir") == 0) {
		read_directory_tree(0);
	} else if (strcmp(i1,"read") == 0) {
		unsigned char* buf = read_file(0, atoi(iargs[0]));
		//printf("%s\n", buf);
		if (buf)
			free(buf);
	} else {
		printf("Not a known command or program!\n");
	}

    printf("%s> ", current_directory);
}
