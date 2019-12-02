#include <ctype.h>

int isgraph(int c) {
	if (isprint(c) && c != ' ')
		return 1;
	else
		return 0;
}
