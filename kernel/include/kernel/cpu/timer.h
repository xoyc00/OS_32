#ifndef _TIMER_H
#define _TIMER_H

#include <stdint.h>

void timer_callback();
void timer_init(uint32_t freq);
void sleep(uint32_t ms);
uint32_t timer_get_ticks();

#endif
