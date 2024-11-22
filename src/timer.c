#include "timer.h"
#include <avr/interrupt.h>

static timer_t *g_timer;
static uint16_t g_tcnt1;

ISR (TIMER1_OVF_vect)
{
  g_timer->timer_callback(g_timer->callback_data);
	TCNT1 = g_tcnt1;
}

void timer_start(timer_t *timer)
{
  g_timer = timer;
  g_tcnt1 = 65536 - ((timer->period_ms * 1000) >> 6);
  TCNT1 = g_tcnt1;
	TCCR1A = 0x00;
	TCCR1B = (1 << CS10) | (1 << CS12);
	TIMSK1 = (1 << TOIE1);
}

void timer_stop(void)
{
	TIMSK1 = 0;
}