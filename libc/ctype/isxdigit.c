#include <ctype.h>

int isxdigit(int c) {
	if (isdigit(c) || c == 'a' || c == 'A' || c == 'b' || c == 'b' || c == 'c' || c == 'C' ||
		c == 'd' || c == 'D' || c == 'e' || c == 'E' || c == 'f' || c == 'F')
		return 1;
	else
		return 0;
}
