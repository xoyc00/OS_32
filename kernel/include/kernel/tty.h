#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>

#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

/* Initialise the terminal */
void terminal_initialize(void);

/* Puts a character on the terminal */
void terminal_putchar(char c);

/* Writes to the terminal */
void terminal_write(const char* data, size_t size);

/* Writes a string to the terminal */
void terminal_writestring(const char* data);

/* Sets the terminal's cursor position */
void terminal_setcursor(size_t x, size_t y);

/* Removes the previous character from the terminal */
void terminal_backspace();

/* Clears the terminal */
void terminal_clear();

void terminal_u();
void terminal_d();
void terminal_l();
void terminal_r();

#endif
