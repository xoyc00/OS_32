#include <stdio.h>

#include <kernel/tty.h>

void kernel_main(void) {
	terminal_initialize();
	printf("Hello, kernel World!\n");
	printf("Created by Millie\n");
	printf("Test Number: %f\n", 1.23456);
	printf("> ");
}
