#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort(void);

char* itoa(long long int value, char* str, int base);
char* ftoa(double value, char* str, int precision);
int atoi(const char* str);
double atof(const char *str);

// Memory functions
void* malloc(size_t size);
void free(void* ptr);

#ifdef __cplusplus
}
#endif

#endif
