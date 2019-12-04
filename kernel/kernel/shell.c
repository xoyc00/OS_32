#include <kernel/shell.h>

#include <kernel/memory_mapper.h>
#include <kernel/driver/ata.h>
#include <kernel/tty.h>

#include <string.h>
#include <stdio.h>

void process_input(char* input) {
	char* i1 = strtok(input, " ");
	char* iargs[64];
	for (int i = 0; i < 64; i++) {
		char* arg = strtok(0, " ");
		if (arg) {
			iargs[i] = arg;
		} else {
			break;
		}
	}

    if (strcmp(i1, "shutdown") == 0) {
        printf("Stopping the CPU. It is now safe to turn off your computer.\n");
        asm volatile("hlt");
    } else if (strcmp (i1, "clear") == 0) {
		terminal_clear();
	} else if (strcmp(i1, "memmap") == 0) {
		print_memory_map();
	} else if (strcmp(i1, "drvlist") == 0) {
		ide_list();
	}else if (strcmp(i1, "chdir") == 0) {
		if (iargs[0]) {
			printf("Changing directory to %s\n", iargs[0]);
		}
	}

    printf("> ");
}
