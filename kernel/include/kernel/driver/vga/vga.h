#ifndef _VGA_H
#define _VGA_H

#include <stddef.h>

void vga_init(size_t width, size_t height, size_t pitch, size_t bpp, size_t addr);

void vga_drawcursor();
void vga_clearscreen(unsigned char r, unsigned char g, unsigned char b);
void vga_putpixel(size_t x, size_t y, unsigned char r, unsigned char g, unsigned char b);
void vga_drawchar(char c, int x, int y, char r, char g, char b);
void vga_drawstr(const char* str, int x, int y, char r, char g, char b);
void vga_terminal_write(char* str, size_t size);
void vga_terminal_drawstr(char* str, size_t size);
void vga_terminal_backspace();
void vga_terminal_draw();
void vga_terminal_drawcharat(unsigned char c, size_t x, size_t y);
void vga_swapbuffers();
void vga_terminal_clear();

#endif
