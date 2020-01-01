#include <kernel/driver/vga/vga.h>
#include <kernel/driver/vga/vga_font.h>
#include <kernel/driver/mouse.h>
#include <kernel/driver/pcspkr.h>
#include <kernel/driver/fat32.h>
#include <kernel/cpu/timer.h>
#include "font.c"

#include <kernel/system/window_manage.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

size_t vga_width, vga_height, vga_pitch, vga_bpp;
unsigned char* vga_mem;
unsigned char backbuffer_mem[1280*1024*4];

static size_t terminal_row;
static size_t terminal_column;

int vga_drvr_enabled = 0;
int display_cursor = 0;
int vga_drvr_finished = 0;

#define TERMINAL_HEIGHT 60
#define TERMINAL_WIDTH 	160

unsigned char terminal_mem[TERMINAL_HEIGHT * TERMINAL_WIDTH];

/* Initiliases the VGA driver. */
void vga_init(size_t width, size_t height, size_t pitch, size_t bpp, size_t addr) {
	vga_width = width;
	vga_height = height;
	vga_pitch = pitch;
	vga_bpp = bpp;
	vga_mem = (unsigned char*)addr;

	memset(backbuffer_mem, 0, 1280*1024*4);
	memset(terminal_mem, 0, TERMINAL_HEIGHT * TERMINAL_WIDTH);

	terminal_row = 0;
	terminal_column = 0;
	vga_drvr_enabled = 1;
}

/* Clears the back buffer. */
void vga_clearscreen(unsigned char r, unsigned char g, unsigned char b) {
	for (size_t x = 0; x < vga_width; x++) {
		for (size_t y = 0; y < vga_height; y++) {
			vga_putpixel(x, y, r, g, b);
		}
	}
}

/* Puts a single pixel to the back buffer. */
void vga_putpixel(size_t x, size_t y, unsigned char r, unsigned char g, unsigned char b) {
	if (x < 0) return;
	if (x > vga_width) return;
	if (y < 0) return;
	if (y > vga_height) return;

	unsigned int bytes_per_line = vga_width * (vga_bpp / 8);

	*(backbuffer_mem + (y * bytes_per_line) + (x * (vga_bpp / 8)) + 0) = b;
	*(backbuffer_mem + (y * bytes_per_line) + (x * (vga_bpp / 8)) + 1) = g;
	*(backbuffer_mem + (y * bytes_per_line) + (x * (vga_bpp / 8)) + 2) = r;
}

/* Draws the mouse cursor to the back buffer. */
void vga_drawcursor() {
	int mouse_x = 0;
	int mouse_y = 0;
	mouse_getposition(&mouse_x, &mouse_y);

	for (int y = 0; y < 16; y++) {
		if (font_cursor[(y*16)+0] == 'X')
			vga_putpixel(mouse_x+0, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*16)+1] == 'X')
			vga_putpixel(mouse_x+1, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*16)+2] == 'X')
			vga_putpixel(mouse_x+2, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*16)+3] == 'X')
			vga_putpixel(mouse_x+3, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*16)+4] == 'X')
			vga_putpixel(mouse_x+4, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*16)+5] == 'X')
			vga_putpixel(mouse_x+5, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*16)+6] == 'X')
			vga_putpixel(mouse_x+6, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*16)+7] == 'X')
			vga_putpixel(mouse_x+7, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*16)+8] == 'X')
			vga_putpixel(mouse_x+8, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*16)+9] == 'X')
			vga_putpixel(mouse_x+9, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*16)+10] == 'X')
			vga_putpixel(mouse_x+10, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*16)+11] == 'X')
			vga_putpixel(mouse_x+11, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*16)+12] == 'X')
			vga_putpixel(mouse_x+10, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*16)+13] == 'X')
			vga_putpixel(mouse_x+13, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*16)+14] == 'X')
			vga_putpixel(mouse_x+14, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*16)+15] == 'X')
			vga_putpixel(mouse_x+15, mouse_y+y, 255, 255, 255);

		if (font_cursor[(y*16)+0] == 'b')
			vga_putpixel(mouse_x+0, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*16)+1] == 'b')
			vga_putpixel(mouse_x+1, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*16)+2] == 'b')
			vga_putpixel(mouse_x+2, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*16)+3] == 'b')
			vga_putpixel(mouse_x+3, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*16)+4] == 'b')
			vga_putpixel(mouse_x+4, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*16)+5] == 'b')
			vga_putpixel(mouse_x+5, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*16)+6] == 'b')
			vga_putpixel(mouse_x+6, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*16)+7] == 'b')
			vga_putpixel(mouse_x+7, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*16)+8] == 'b')
			vga_putpixel(mouse_x+8, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*16)+9] == 'b')
			vga_putpixel(mouse_x+9, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*16)+10] == 'b')
			vga_putpixel(mouse_x+10, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*16)+11] == 'b')
			vga_putpixel(mouse_x+11, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*16)+12] == 'b')
			vga_putpixel(mouse_x+12, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*16)+13] == 'b')
			vga_putpixel(mouse_x+13, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*16)+14] == 'b')
			vga_putpixel(mouse_x+14, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*16)+15] == 'b')
			vga_putpixel(mouse_x+15, mouse_y+y, 0, 0, 0);
	}
}

/* Draws a single character to the back buffer. */
void vga_drawchar(char c, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
	for (int row = 0; row < 16; row ++) {
		unsigned char character = font.Bitmap[(int)c * 16 + row];
		if (character & (1 << 7)) vga_putpixel(x+0, y+row, r, g, b);
		if (character & (1 << 6)) vga_putpixel(x+1, y+row, r, g, b);
		if (character & (1 << 5)) vga_putpixel(x+2, y+row, r, g, b);
		if (character & (1 << 4)) vga_putpixel(x+3, y+row, r, g, b);
		if (character & (1 << 3)) vga_putpixel(x+4, y+row, r, g, b);
		if (character & (1 << 2)) vga_putpixel(x+5, y+row, r, g, b);
		if (character & (1 << 1)) vga_putpixel(x+6, y+row, r, g, b);
		if (character & (1 << 0)) vga_putpixel(x+7, y+row, r, g, b);
	}
}

/* Draws a string of characters to the back buffer. */
void vga_drawstr(const char* str, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
	int i = 0;
	while(str[i] != '\0') {
		vga_drawchar(str[i], x + (i * 8), y, r, g, b);
		i ++;
	}
}

/* Draws a rectangle with optionally rounded edges to the back buffer. */
void vga_drawrect(int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b, int rounded) {
	unsigned char* where = backbuffer_mem + (y*vga_width*(vga_bpp/8)) + (x*(vga_bpp/8));
	int i, j;

	for (i = 0; i < h; i++) {
		if (i+y > 0 && i+y < vga_height) {
			for (j = 0; j < w; j++) {
				if (j+x > 0 && j+x < vga_width) {
					if (rounded) {
						if ((j < 1 || j > w-2) && (i < 1 || i > h-2)) continue;
					}
					
					where[j*4+0] = b;
					where[j*4+1] = g;
					where[j*4+2] = r;
				}
			}
		}
		where += vga_width*(vga_bpp/8);
	}
}

/* Writes a string of a given length to the terminal. */
void vga_terminal_write(char* str, size_t size) {
	vga_terminal_drawstr(str, size);
}

/* Writes a character to the terminal at a given location. */
void vga_terminal_drawcharat(unsigned char c, size_t x, size_t y) {
	terminal_mem[x + (y*TERMINAL_WIDTH)] = c;
}

/* Writes a string to the terminal. */
void vga_terminal_drawstr(char* str, size_t size) {
	for (int i = 0; i < size; i++) {
		if (str[i] == '\n') {
			terminal_column = 0;
			terminal_row ++;

			if (terminal_row == TERMINAL_HEIGHT) {
				// scroll the screen
				int i;
		    	for (i = 1; i < TERMINAL_HEIGHT; i++) {
					uint16_t* src = &terminal_mem[i * TERMINAL_WIDTH];
					uint16_t* dst = &terminal_mem [((i - 1) * TERMINAL_WIDTH)];
					memcpy(dst, src, TERMINAL_WIDTH);
				}

				// Clear the bottom line
				for (i = 0; i < (int)TERMINAL_WIDTH; i++)
					vga_terminal_drawcharat(' ', (size_t)i, TERMINAL_HEIGHT-1);

				terminal_row --;
			}
		} else {
			terminal_mem[(terminal_row*TERMINAL_WIDTH) + terminal_column] = str[i];
			terminal_column++;

			if (terminal_column == TERMINAL_WIDTH) {
				terminal_column = 0;
				terminal_row ++;
			}

			if (terminal_row == TERMINAL_HEIGHT) {
				// scroll the screen
				int i;
		    	for (i = 1; i < TERMINAL_HEIGHT; i++) {
					uint16_t* src = &terminal_mem[i * TERMINAL_WIDTH];
					uint16_t* dst = &terminal_mem [((i - 1) * TERMINAL_WIDTH)];
					memcpy(dst, src, TERMINAL_WIDTH);
				}

				// Clear the bottom line
				for (i = 0; i < (int)TERMINAL_WIDTH; i++)
					vga_terminal_drawcharat(' ', (size_t)i, TERMINAL_HEIGHT-1);

				terminal_row --;
			}
		}	

		if (terminal_column == TERMINAL_WIDTH) {
			terminal_column = 0;
			terminal_row ++;
		}

		if (terminal_row == TERMINAL_HEIGHT) {
			// scroll the screen
			int i;
		    for (i = 1; i < TERMINAL_HEIGHT; i++) {
				uint16_t* src = &terminal_mem[i * TERMINAL_WIDTH];
				uint16_t* dst = &terminal_mem [((i - 1) * TERMINAL_WIDTH)];
				memcpy(dst, src, TERMINAL_WIDTH);
			}

			// Clear the bottom line
			for (i = 0; i < (int)TERMINAL_WIDTH; i++)
				vga_terminal_drawcharat(' ', (size_t)i, TERMINAL_HEIGHT-1);

			terminal_row --;
		}
	}
}

/* Draws the terminal to the back buffer. */
void vga_terminal_draw() {
	for (int y = 0; y < TERMINAL_HEIGHT; y++) {
		for (int x = 0; x < TERMINAL_WIDTH; x++) {
			unsigned char c = terminal_mem[x + (y * TERMINAL_WIDTH)];
			vga_drawchar(c, x*8, y*16, 194, 188, 166);
		}
	}

	if (display_cursor) {
		vga_drawrect((terminal_column)*8, (terminal_row)*16, 8, 16, 194, 188, 166, 0);
	}
}

/* Processes a backspace in the terminal. */
void vga_terminal_backspace() {
	if (terminal_column <= 2 + strlen(current_directory)) {
		pcspkr_beep();
		return;
	}
	terminal_column--;
	vga_terminal_drawstr(" ", 1);
	terminal_column--;
}

/* TODO: vertical sync */
/* Handles swapping the VGA buffers (front and back buffer) to the screen when V-Sync is finished. */
void vga_swap_handler() {
	if (!vga_drvr_finished) return;

	vga_drvr_finished = 0;

	unsigned int bytes_per_line = vga_width * (vga_bpp / 8);
	uint8_t* dest = vga_mem;
	uint8_t* src  = &backbuffer_mem[0];

	for (size_t y = 0; y < vga_height; y++) {
		memcpy(dest, src, bytes_per_line);
		dest += vga_pitch;
		src += bytes_per_line;
	}
}

/* Swaps the VGA buffers. */
void vga_swapbuffers() {
	vga_drvr_finished = 1;
	vga_swap_handler();			// TODO: Vertical Sync
}

/* Clears the terminal. */
void vga_terminal_clear() {
	terminal_column = 0;
	terminal_row = 0;
	
	for (size_t x = 0; x < TERMINAL_WIDTH; x++) {
		for  (int y = 0; y < TERMINAL_HEIGHT; y++) {
			vga_terminal_drawcharat(' ', x, y);
		}
	}
}

/* Blits a memory buffer onto the screen. */
void vga_blit_buffer(const unsigned char* buffer, int x, int y, int w, int h, int bpp) {
	bpp /= 8;

	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			vga_putpixel(i+x, j+y, buffer[(i*bpp)+(j*w*bpp) + 2], buffer[(i*bpp)+(j*w*bpp) + 1], buffer[(i*bpp)+(j*w*bpp) + 0]);
		}
	}
}

/* Draws a window defined by the window manager. */
void vga_drawwindow(window_t window) {
	vga_drawrect(window.x - window.border_radius, window.y - window.border_radius, window.w + (window.border_radius * 2), window.h + (window.tb_h - 2) + (window.border_radius * 2), 194, 188, 166, window.rounded);
	vga_drawrect(window.x, window.y, window.w, window.tb_h, 194, 188, 166, window.rounded);
	vga_drawstr(window.title, window.x + 1, window.y + 1, 255, 255, 255);
	vga_drawrect(window.x + window.w - 16, window.y + 1, 14, 14, 255, 0, 0, 0);

	// Draw the framebuffer
	vga_blit_buffer(window.framebuffer, window.x, window.y + window.tb_h - 2, window.w, window.h, 32);
}

/* Puts a character into a window at a given location. */
void wm_putchar(window_t* w, unsigned char c, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
	for (int row = 0; row < 16; row ++) {
		unsigned char character = font.Bitmap[(int)c * 16 + row];
		if (character & (1 << 7)) wm_putpixel(w, x+0, y+row, r, g, b, 255);
		if (character & (1 << 6)) wm_putpixel(w, x+1, y+row, r, g, b, 255);
		if (character & (1 << 5)) wm_putpixel(w, x+2, y+row, r, g, b, 255);
		if (character & (1 << 4)) wm_putpixel(w, x+3, y+row, r, g, b, 255);
		if (character & (1 << 3)) wm_putpixel(w, x+4, y+row, r, g, b, 255);
		if (character & (1 << 2)) wm_putpixel(w, x+5, y+row, r, g, b, 255);
		if (character & (1 << 1)) wm_putpixel(w, x+6, y+row, r, g, b, 255);
		if (character & (1 << 0)) wm_putpixel(w, x+7, y+row, r, g, b, 255);
	}
}

/* Puts a string into a window at a given location. */
void wm_putstr(window_t* w, unsigned char* str, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
	int i = 0;
	while(str[i] != '\0') {
		wm_putchar(w, str[i], x + (i * 8), y, r, g, b);
		i ++;
	}
}

/* Loads a .BMP image to a memory buffer. */
unsigned char* vga_load_bitmap_to_buffer(char* path, int *w, int *h, int *bpp) {
	int size;

	unsigned char* bmp_total = read_file_from_name(0, path);
	if (bmp_total == 0) return 0;

	// Data read from the header of the BMP file
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data
	unsigned char * data;

	if (bmp_total[0] != 'B' || bmp_total[1] != 'M') {
		printf("WARNING: Not a BMP file!");
	}

	dataPos    = *(int*)&(bmp_total[0x0A]);
	imageSize  = *(int*)&(bmp_total[0x22]);
	width      = *(int*)&(bmp_total[0x12]);
	height     = *(int*)&(bmp_total[0x16]);
	if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)      dataPos=54; // The BMP header is done that way

	data = (unsigned char*)((uint32_t)bmp_total + dataPos);

	*w = width;
	*h = height;
	*bpp = 24;
	return data;
}
