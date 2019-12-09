#ifndef _MOUSE_H
#define _MOUSE_H

/* Callback for when the mouse is moved or a button is pressed */
void mouse_handler();

/* Initialises the mouse */
void mouse_init();

/* Puts the mouse position into x and y */
void mouse_getposition(int* x, int* y);

#endif
