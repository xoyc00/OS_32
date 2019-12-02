#include <ctype.h>

int isalpha(int c) {
	if (isupper(c) || islower(c))
		return 1;
	else
		return 0;
}
