#ifndef _TIMER_H
#define _TIMER_H

#include <stdint.h>

/* Callback for when the timer ticks */
void timer_callback();

/* Initialise the timer at a given frequency */
void timer_init(uint32_t freq);

/* Sleep for a given time */
void sleep(uint32_t ms);

/* Return the current tick count */
uint32_t timer_get_ticks();

#endif
