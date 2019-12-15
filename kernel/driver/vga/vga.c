#include <kernel/driver/vga/vga.h>
#include <kernel/driver/vga/vga_font.h>
#include <kernel/driver/mouse.h>
#include <kernel/driver/pcspkr.h>
#include <kernel/driver/fat32.h>
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
int vga_drvr_finished = 0;

#define TERMINAL_HEIGHT 64
#define TERMINAL_WIDTH 	160

unsigned char terminal_mem[TERMINAL_HEIGHT * TERMINAL_WIDTH];

void vga_init(size_t width, size_t height, size_t pitch, size_t bpp, size_t addr) {
	vga_width = width;
	vga_height = height;
	vga_pitch = pitch;
	vga_bpp = bpp;
	vga_mem = (unsigned char*)addr;

	memset(backbuffer_mem, 0, 1024*768*4);

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

void vga_drawstr(const char* str, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
	int i = 0;
	while(str[i] != '\0') {
		vga_drawchar(str[i], x + (i * 8), y, r, g, b);
		i ++;
	}
}

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

void vga_terminal_write(char* str, size_t size) {
	vga_terminal_drawstr(str, size);
}

void vga_terminal_drawcharat(unsigned char c, size_t x, size_t y) {
	terminal_mem[x + (y*TERMINAL_WIDTH)] = c;
}

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

void vga_terminal_draw() {
	for (int y = 0; y < TERMINAL_HEIGHT; y++) {
		for (int x = 0; x < TERMINAL_WIDTH; x++) {
			unsigned char c = terminal_mem[x + (y * TERMINAL_WIDTH)];
			vga_drawchar(c, x*8, y*16, 0, 255, 0);
		}
	}

	if (display_cursor) {
		vga_putpixel(terminal_column*8+0,terminal_row*16+15, 0, 255, 0); 
		vga_putpixel(terminal_column*8+1,terminal_row*16+15, 0, 255, 0); 
		vga_putpixel(terminal_column*8+2,terminal_row*16+15, 0, 255, 0); 
		vga_putpixel(terminal_column*8+3,terminal_row*16+15, 0, 255, 0); 
		vga_putpixel(terminal_column*8+4,terminal_row*16+15, 0, 255, 0); 
		vga_putpixel(terminal_column*8+5,terminal_row*16+15, 0, 255, 0); 
		vga_putpixel(terminal_column*8+6,terminal_row*16+15, 0, 255, 0); 
		vga_putpixel(terminal_column*8+7,terminal_row*16+15, 0, 255, 0); 

		vga_putpixel(terminal_column*8+0,terminal_row*16+16, 0, 255, 0); 
		vga_putpixel(terminal_column*8+1,terminal_row*16+16, 0, 255, 0); 
		vga_putpixel(terminal_column*8+2,terminal_row*16+16, 0, 255, 0); 
		vga_putpixel(terminal_column*8+3,terminal_row*16+16, 0, 255, 0); 
		vga_putpixel(terminal_column*8+4,terminal_row*16+16, 0, 255, 0); 
		vga_putpixel(terminal_column*8+5,terminal_row*16+16, 0, 255, 0); 
		vga_putpixel(terminal_column*8+6,terminal_row*16+16, 0, 255, 0); 
		vga_putpixel(terminal_column*8+7,terminal_row*16+16, 0, 255, 0); 
	}
}

void vga_terminal_backspace() {
	if (terminal_column <= 2 + strlen(current_directory)) {
		pcspkr_beep();
		return;
	}
	terminal_column--;
	vga_terminal_drawstr(" ", 1);
	terminal_column--;
}

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

void vga_swapbuffers() {
	vga_drvr_finished = 1;
	vga_swap_handler();			// TODO: Vertical Sync
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

void vga_blit_buffer(unsigned char* buffer, int x, int y, int w, int h) {
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			if (buffer[x*4+y*w + 3] > 127) vga_putpixel(i+x, j+y, buffer[x*4+y*w + 0], buffer[x*4+y*w + 1], buffer[x*4+y*w + 2]);
		}
	}
}

void vga_drawwindow(window_t window) {
	vga_drawrect(window.x - window.border_radius, window.y - window.border_radius, window.w + (window.border_radius * 2), window.h + (window.tb_h - 2) + (window.border_radius * 2), 32, 32, 255, window.rounded);
	vga_drawrect(window.x, window.y + window.tb_h - 2, window.w, window.h, window.bg_r, window.bg_g, window.bg_b, window.rounded);
	vga_drawrect(window.x, window.y, window.w, window.tb_h, 32, 32, 255, window.rounded);
	vga_drawstr(window.title, window.x + 1, window.y + 1, 255, 255, 255);
	vga_drawrect(window.x + window.w - 16, window.y + 2, 14, 14, 255, 0, 0, 0);

	// Draw the framebuffer
	vga_blit_buffer(window.framebuffer, window.x, window.y + window.tb_h - 2, window.w, window.h);
}
