#include <kernel/driver/pcspkr.h>

#include <kernel/cpu/timer.h>
#include <kernel/cpu/ports.h>

#include <assert.h>

void pcspkr_play_sound(uint32_t freq) {
	uint32_t Div;
 	uint8_t tmp;
 
        //Set the PIT to the desired frequency
 	Div = 1193180 / freq;
 	outb(0x43, 0xb6);
 	outb(0x42, (uint8_t) (Div) );
 	outb(0x42, (uint8_t) (Div >> 8));
 
        //And play the sound using the PC speaker
 	tmp = inb(0x61);
  	if (tmp != (tmp | 3)) {
 		outb(0x61, tmp | 3);
 	}
}

void pcspkr_stop_sound() {
	uint8_t tmp = inb(0x61) & 0xFC;
 	outb(0x61, tmp);
}

void pcspkr_beep() {
	pcspkr_play_sound(880);
	asm("sti");
	sleep(100);
	pcspkr_stop_sound();
}

