#include <kernel/shell.h>

#include <kernel/memory_mapper.h>
#include <kernel/tty.h>

#include <string.h>
#include <stdio.h>

void process_input(char* input) {
	char* i1 = strtok(input, " ");

    if (strcmp(input, "shutdown") == 0) {
        printf("Stopping the CPU. It is now safe to turn off your computer.\n");
        asm volatile("hlt");
    } else if (strcmp (input, "clear") == 0) {
		terminal_clear();
	} else if (strcmp(input, "memmap") == 0) {
		print_memory_map();
	} else if (strcmp(i1, "chdir") == 0) {
		char* directory = strtok(0, " ");
		if (directory) {
			printf("Changing directory to %s\n", directory);
		}
	}

    printf("> ");
}
