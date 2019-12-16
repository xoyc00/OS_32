#ifndef _WINDOW_MANAGE_H
#define _WINDOW_MANAGE_H

typedef struct window {
	int wid;							// window ID
	int tid;							// parent task ID

	int x, y;							// x and y position
	int w, h;							// width and height
	int tb_h;							// title bar height
	int border_radius;					// How many pixels should the border be?
	int rounded;						// should the window have rounded edges?
	int depth;

	unsigned char bg_r, bg_g, bg_b;		// background rgb

	char* title;						// title

	unsigned char* framebuffer;
} window_t;

void wm_mouse_button_down(int button, int x, int y);
void wm_mouse_button_up(int button, int x, int y);

// Initialise the window manager
void wm_init();

// Draw the windows to the screen
void wm_draw();

window_t* window_create(int w, int h, char* title);
void window_register(window_t* w);
void window_deregister(window_t* w);

void wm_clearwindow(window_t* w);
void wm_putpixel(window_t* w, int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void wm_putchar(window_t* w, unsigned char c, int x, int y, unsigned char r, unsigned char g, unsigned char b);				// implemented in vga.c
void wm_putstr(window_t* w, unsigned char* str, int x, int y, unsigned char r, unsigned char g, unsigned char b);			// implemented in vga.c

#endif
