#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>
#include <kernel/cpu/ports.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = (y * VGA_WIDTH) + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
	unsigned char uc = c;
	if (uc == '\n') {
		terminal_row ++;
		terminal_column = -1;
	}
	else
		terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);

	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}

	if (terminal_row == VGA_HEIGHT) {
		// Scroll the screen
		int i;
        for (i = 1; i < VGA_HEIGHT; i++) {
			uint16_t* src = terminal_buffer + (i * VGA_WIDTH);
			uint16_t* dst = terminal_buffer + ((i - 1) * VGA_WIDTH);
			memcpy(dst, src, VGA_WIDTH);
		}

		// Clear the bottom line
		for (i = 0; i < (int)VGA_WIDTH; i++)
			terminal_putentryat(' ', terminal_color, (size_t)i, VGA_HEIGHT-1);

		terminal_row --;
	}

	terminal_setcursor(terminal_column, terminal_row);
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}

void terminal_setcursor(size_t x, size_t y) {
	terminal_column = x;
	terminal_row = y;

	int offset = (x + (y*VGA_WIDTH));
	outb(REG_SCREEN_CTRL, 14);
	outb(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
	outb(REG_SCREEN_CTRL, 15);
	outb(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));
}

void terminal_backspace() {
	if (terminal_column == 0) return;
	terminal_setcursor(terminal_column - 1, terminal_row);
	terminal_putchar(' ');
	terminal_setcursor(terminal_column - 1, terminal_row);
}
