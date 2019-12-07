#include <kernel/driver/vga/vga.h>
#include <kernel/driver/vga/vga_font.h>
#include <kernel/driver/mouse.h>
#include <kernel/driver/pcspkr.h>
#include <kernel/cpu/timer.h>
#include "font.c"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

size_t vga_width, vga_height, vga_pitch, vga_bpp;
unsigned char* vga_mem;
unsigned char backbuffer_mem[1280*1024*4];

static size_t terminal_row;
static size_t terminal_column;

int vga_drvr_enabled = 0;
int display_cursor = 0;

#define TERMINAL_HEIGHT 64
#define TERMINAL_WIDTH 160

unsigned char terminal_mem[TERMINAL_HEIGHT * TERMINAL_WIDTH];

void vga_init(size_t width, size_t height, size_t pitch, size_t bpp, size_t addr) {
	vga_width = width;
	vga_height = height;
	vga_pitch = pitch;
	vga_bpp = bpp;
	vga_mem = (unsigned char*)addr;

	memset(backbuffer_mem, 0, sizeof(backbuffer_mem));

	terminal_row = 0;
	terminal_column = 0;
	vga_drvr_enabled = 1;
}

void vga_clearscreen(unsigned char r, unsigned char g, unsigned char b) {
	for (size_t x = 0; x < vga_width; x++) {
		for (size_t y = 0; y < vga_height; y++) {
			vga_putpixel(x, y, r, g, b);
		}
	}
}

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

void vga_drawcursor() {
	int mouse_x = 0;
	int mouse_y = 0;
	mouse_getposition(&mouse_x, &mouse_y);

	for (int y = 0; y < 12; y++) {
		if (font_cursor[(y*12)+0] == 'X')
			vga_putpixel(mouse_x+0, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*12)+1] == 'X')
			vga_putpixel(mouse_x+1, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*12)+2] == 'X')
			vga_putpixel(mouse_x+2, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*12)+3] == 'X')
			vga_putpixel(mouse_x+3, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*12)+4] == 'X')
			vga_putpixel(mouse_x+4, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*12)+5] == 'X')
			vga_putpixel(mouse_x+5, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*12)+6] == 'X')
			vga_putpixel(mouse_x+6, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*12)+7] == 'X')
			vga_putpixel(mouse_x+7, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*12)+8] == 'X')
			vga_putpixel(mouse_x+8, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*12)+9] == 'X')
			vga_putpixel(mouse_x+9, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*12)+10] == 'X')
			vga_putpixel(mouse_x+10, mouse_y+y, 255, 255, 255);
		if (font_cursor[(y*12)+11] == 'X')
			vga_putpixel(mouse_x+11, mouse_y+y, 255, 255, 255);

		if (font_cursor[(y*12)+0] == 'b')
			vga_putpixel(mouse_x+0, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*12)+1] == 'b')
			vga_putpixel(mouse_x+1, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*12)+2] == 'b')
			vga_putpixel(mouse_x+2, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*12)+3] == 'b')
			vga_putpixel(mouse_x+3, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*12)+4] == 'b')
			vga_putpixel(mouse_x+4, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*12)+5] == 'b')
			vga_putpixel(mouse_x+5, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*12)+6] == 'b')
			vga_putpixel(mouse_x+6, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*12)+7] == 'b')
			vga_putpixel(mouse_x+7, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*12)+8] == 'b')
			vga_putpixel(mouse_x+8, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*12)+9] == 'b')
			vga_putpixel(mouse_x+9, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*12)+10] == 'b')
			vga_putpixel(mouse_x+10, mouse_y+y, 0, 0, 0);
		if (font_cursor[(y*12)+11] == 'b')
			vga_putpixel(mouse_x+11, mouse_y+y, 0, 0, 0);
	}
}

void vga_drawchar(char c, int x, int y, char r, char g, char b) {
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

void vga_drawstr(const char* str, int x, int y, char r, char g, char b) {
	int i = 0;
	while(*str) {
		vga_drawchar(*str, x + i, y, r, g, b);
		str++;
		i = i + 8;
	}
}

void vga_terminal_write(char* str, size_t size) {
	vga_terminal_drawstr(str, size);
}

void vga_terminal_drawcharat(unsigned char c, size_t x, size_t y) {
	terminal_mem[(y*TERMINAL_WIDTH) + x] = c;
}

void vga_terminal_drawstr(char* str, size_t size) {
	for (int i = 0; i < size; i++) {
		if (str[i] == '\n') {
			terminal_column = 0;
			terminal_row ++;
		} else {
			terminal_mem[(terminal_row*TERMINAL_WIDTH) + terminal_column] = str[i];
			terminal_column++;
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
			for (i = 0; i < (int)TERMINAL_HEIGHT; i++)
				vga_terminal_drawcharat(' ', (size_t)i, TERMINAL_HEIGHT-1);

			terminal_row --;
		}
	}
}

void vga_terminal_draw() {
	for (int x = 0; x < TERMINAL_WIDTH; x++) {
		for (int y = 0; y < TERMINAL_HEIGHT; y++) {
			unsigned char c = terminal_mem[x + (y * TERMINAL_WIDTH)];
			vga_drawchar(c, x*8, y*16, 255, 255, 255);
		}
	}

	if (display_cursor) {
		vga_putpixel(terminal_column*8+0,terminal_row*16+15, 255, 255, 255); 
		vga_putpixel(terminal_column*8+1,terminal_row*16+15, 255, 255, 255); 
		vga_putpixel(terminal_column*8+2,terminal_row*16+15, 255, 255, 255); 
		vga_putpixel(terminal_column*8+3,terminal_row*16+15, 255, 255, 255); 
		vga_putpixel(terminal_column*8+4,terminal_row*16+15, 255, 255, 255); 
		vga_putpixel(terminal_column*8+5,terminal_row*16+15, 255, 255, 255); 
		vga_putpixel(terminal_column*8+6,terminal_row*16+15, 255, 255, 255); 
		vga_putpixel(terminal_column*8+7,terminal_row*16+15, 255, 255, 255); 

		vga_putpixel(terminal_column*8+0,terminal_row*16+16, 255, 255, 255); 
		vga_putpixel(terminal_column*8+1,terminal_row*16+16, 255, 255, 255); 
		vga_putpixel(terminal_column*8+2,terminal_row*16+16, 255, 255, 255); 
		vga_putpixel(terminal_column*8+3,terminal_row*16+16, 255, 255, 255); 
		vga_putpixel(terminal_column*8+4,terminal_row*16+16, 255, 255, 255); 
		vga_putpixel(terminal_column*8+5,terminal_row*16+16, 255, 255, 255); 
		vga_putpixel(terminal_column*8+6,terminal_row*16+16, 255, 255, 255); 
		vga_putpixel(terminal_column*8+7,terminal_row*16+16, 255, 255, 255); 
	}
}

void vga_terminal_backspace() {
	if (terminal_column <= 2) {
		pcspkr_beep();
		return;
	}
	terminal_column--;
	vga_terminal_drawstr(" ", 1);
	terminal_column--;
}

void vga_swapbuffers() {
	unsigned int bytes_per_line = vga_width * (vga_bpp / 8);
	uint8_t* dest = vga_mem;
	uint8_t* src  = &backbuffer_mem[0];

	for (size_t y = 0; y < vga_height; y++) {
		memcpy(dest, src, bytes_per_line);
		dest += vga_pitch;
		src += bytes_per_line;
	}
}

void vga_terminal_clear() {
	terminal_column = 0;
	terminal_row = 0;
	
	for (size_t x = 0; x < TERMINAL_WIDTH; x++) {
		for  (int y = 0; y < TERMINAL_HEIGHT; y++) {
			vga_terminal_drawcharat(' ', x, y);
		}
	}
}
