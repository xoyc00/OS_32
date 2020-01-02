#ifndef _INTERP_H
#define _INTERP_H

typedef struct variable {
	unsigned char* name;
	unsigned char* type;
	void* data;
} variable_t;

typedef struct label {
	unsigned char* name;
	int line;
} label_t;

extern char input_buf[1024];

void interp(unsigned char* buf);

#endif
