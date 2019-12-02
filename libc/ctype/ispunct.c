#include <ctype.h>

int ispunct(int c) {
	if (isgraph(c) && !isalnum(c))
		return 1;
	else
		return 0;
}
