#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

void kassert(char* file, int line) {
	printf("Assertion failure at file: %s line: %i\n", file, line);
	abort();
}
