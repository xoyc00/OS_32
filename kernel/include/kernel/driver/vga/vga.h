#ifndef _VGA_H
#define _VGA_H

#include <kernel/system/window_manage.h>

#include <stddef.h>
#include <stdint.h>

typedef struct {
   uint16_t type;                 		/* Magic identifier            */
   uint32_t size;                       /* File size in bytes          */
   uint16_t reserved1, reserved2;
   uint32_t offset;                     /* Offset to image data, bytes */
} __attribute__((packed)) BMP_header_t;

typedef struct {
   uint32_t size;               /* Header size in bytes      */
   uint32_t width,height;                /* Width and height of image */
   uint16_t planes;       /* Number of colour planes   */
   uint16_t bits;         /* Bits per pixel            */
   uint32_t compression;        /* Compression type          */
   uint32_t imagesize;          /* Image size in bytes       */
   uint32_t xresolution,yresolution;     /* Pixels per meter          */
   uint32_t ncolours;           /* Number of colours         */
   uint32_t importantcolours;   /* Important colours         */
} __attribute__((packed)) BMP_info_header_t;

/* Initialise the screen */
void vga_init(size_t width, size_t height, size_t pitch, size_t bpp, size_t addr);

/* Draw the cursor */
void vga_drawcursor();

/* Clear the screen */
void vga_clearscreen(unsigned char r, unsigned char g, unsigned char b);

/* Put a single pixel on the screen */
void vga_putpixel(size_t x, size_t y, unsigned char r, unsigned char g, unsigned char b);

/* Draw a single character */
void vga_drawchar(char c, int x, int y, unsigned char r, unsigned char g, unsigned char b);

/* Draw a string */
void vga_drawstr(const char* str, int x, int y, unsigned char r, unsigned char g, unsigned char b);

/* Draw a filled rectangle of a given color in a given position */
void vga_drawrect(int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b, int rounded);

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

/* Blit a framebuffer onto the backbuffer */
void vga_blit_buffer(const unsigned char* buffer, int x, int y, int w, int h, int bpp);

/* Draw a framebuffer */
void vga_drawwindow(window_t window);

unsigned char* vga_load_bitmap_to_buffer(char* path, int *w, int *h, int *bpp);

#endif
