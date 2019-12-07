#ifndef _VGA_H
#define _VGA_H

#include <stddef.h>

void vga_init(size_t width, size_t height, size_t pitch, size_t bpp, size_t addr);

void vga_clearscreen(unsigned char r, unsigned char g, unsigned char b);
void vga_putpixel(size_t x, size_t y, unsigned char r, unsigned char g, unsigned char b);
void vga_drawcursor();

#endif
