#ifndef _PCSPKR_H
#define _PCSPKR_H

#include <stdint.h>

/* Play a sound at a given frequency */
void pcspkr_play_sound(uint32_t freq);

/* Stop the current sound */
void pcspkr_stop_sound();

/* Beep */
void pcspkr_beep();

#endif
