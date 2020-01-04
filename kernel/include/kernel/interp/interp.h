#ifndef _INTERP_H
#define _INTERP_H

#define EQUAL_TO		0x00
#define GREATER_THEN 	0b01
#define LESS_THEN		0b10

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
