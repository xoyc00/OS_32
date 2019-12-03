#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort(void);

char* itoa(int value, char* str, int base);
char* ftoa(double value, char* str, int precision);

#ifdef __cplusplus
}
#endif

#endif
