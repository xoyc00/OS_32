#ifndef _PCSPKR_H
#define _PCSPKR_H

#include <stdint.h>

void pcspkr_play_sound(uint32_t freq);
void pcspkr_stop_sound();
void pcspkr_beep();

#endif
