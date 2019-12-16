#ifndef _TERMINAL_H
#define _TERMINAL_H

#include <kernel/system/window_manage.h>

typedef struct terminal_app {
	int id;

	window_t w;
	unsigned char buf[64*24];
} terminal_app_t;

void draw_terminal(int terminal);
terminal_app_t* create_terminal();
void destroy_terminal(int terminal);



#endif
