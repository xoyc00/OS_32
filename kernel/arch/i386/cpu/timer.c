#include <kernel/cpu/timer.h>
#include <kernel/cpu/ports.h>
#include <stdio.h>

uint32_t tick = 0;

void timer_callback() {
	tick = tick + 1;
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
