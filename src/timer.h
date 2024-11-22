#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

typedef struct timer_t {
  uint16_t period_ms;
  void *callback_data;
  void (*timer_callback)(void *);
} timer_t;

void timer_start(timer_t *timer);
void timer_stop(void);

#endif /* TIMER_H */