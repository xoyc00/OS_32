#include <ctype.h>

int isblank(int c) {
	if (c == ' ' || c == '\t')
		return 1;
	else
		return 0;
}
