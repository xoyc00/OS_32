#include <string.h>

char* strcpy(char* dst, const char* src) {
	int i = 0;
	while (src[i] != '\0') {
		dst[i] = src[i];
		i++;
	}
	dst[i] = src[i];
	return dst;
}
