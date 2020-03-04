#include <math.h>

#define PI 3.1415926535897932384650288

double tan (double x) {
	return sin(x)/cos(x);
}

float tanf (float x) {
	return sinf(x)/cosf(x);
}

long double tanl (long double x) {
	return sinl(x)/cosl(x);
}

