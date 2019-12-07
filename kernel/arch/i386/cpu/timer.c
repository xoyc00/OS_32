#include <kernel/cpu/timer.h>
#include <kernel/cpu/ports.h>
#include <kernel/driver/vga/vga.h>
#include <stdio.h>

extern int display_cursor;

uint32_t tick = 0;

void timer_callback() {
	tick = tick + 1;

	if (tick % 20000 == 0) {
		display_cursor = !display_cursor;
	}
}

void timer_init(uint32_t freq) {
	uint32_t divisor = 1193180 / freq;
	uint8_t low = (uint8_t)(divisor & 0xFF);
	uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);
	outb(0x43, 0x36);
	outb(0x40, low);
	outb(0x40, high);
}

void sleep(uint32_t ms) {
	uint32_t end = tick + ms;
	while(tick < end) {asm("nop");}
}

uint32_t timer_get_ticks() {
	return tick;
}
