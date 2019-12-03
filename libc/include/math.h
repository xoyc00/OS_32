#ifndef _MATH_H
#define _MATH_H 1

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

// Trigonometric functions
double cos (double x);
float cosf (float x);
long double cosl (long double x);

double sin (double x);
float sinf (float x);
long double sinl (long double x);

double tan (double x);
float tanf (float x);
long double tanl (long double x);

#ifdef __cplusplus
}
#endif

#endif
