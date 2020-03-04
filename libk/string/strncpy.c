#include <string.h>

char* strncpy(char* dst, const char* src, size_t num) {
	int i = 0;
	while (src[i] != '\0') {
		dst[i] = src[i];
		i++;

		if (i+1 > num) return dst;
	}
	dst[i] = src[i];
	return dst;
}
