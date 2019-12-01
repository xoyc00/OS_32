#include <stdio.h>

#include <kernel/tty.h>

void kernel_main(void) {
	terminal_initialize();
	printf("Hello, kernel World!\n");
	printf("Created by Millie\n");
	printf("Test Number: %f", 1.23456);

	unsigned int i;
	while(1) {
		if (i % 100000000 == 0)
			printf("\nHello!");

		i ++;
	}
}
