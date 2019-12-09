#ifndef _VGA_H
#define _VGA_H

#include <stddef.h>

/* Initialise the screen */
void vga_init(size_t width, size_t height, size_t pitch, size_t bpp, size_t addr);

/* Draw the cursor */
void vga_drawcursor();

/* Clear the screen */
void vga_clearscreen(unsigned char r, unsigned char g, unsigned char b);

/* Put a single pixel on the screen */
void vga_putpixel(size_t x, size_t y, unsigned char r, unsigned char g, unsigned char b);

/* Draw a single character */
void vga_drawchar(char c, int x, int y, char r, char g, char b);

/* Draw a string */
void vga_drawstr(const char* str, int x, int y, char r, char g, char b);

/* Write to the VGA terminal */
void vga_terminal_write(char* str, size_t size);

/* Draw a string to the vga terminal */
void vga_terminal_drawstr(char* str, size_t size);

/* Remove the previous character form the VGA terminal */
void vga_terminal_backspace();

/* Draw the terminal to the screen */
void vga_terminal_draw();

/* Draw a character at a given point on the terminal */
void vga_terminal_drawcharat(unsigned char c, size_t x, size_t y);

/* Clear the VGA terminal */
void vga_terminal_clear();

/* Swap the VGA buffers */
void vga_swapbuffers();

#endif
