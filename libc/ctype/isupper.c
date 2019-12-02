#include <ctype.h>

int isupper(int c) {
	if (isalpha(c) && !islower(c))
		return 1;
	else
		return 0;
}
