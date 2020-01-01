#ifndef _INTERP_H
#define _INTERP_H

typedef struct variable {
	unsigned char* name;
	unsigned char* type;
	void* data;
} variable_t;

void interp(unsigned char* buf);

#endif
