#include <ctype.h>

int iscntrl(int c) {
	if (c >= 0x00 && c <= 0x1f)
		return 1;
	else if (c == 0x7f)
		return 1;
	else
		return 0;
}
