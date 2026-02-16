#ifndef MODE_MIDILEARN_H
#define MODE_MIDILEARN_H

#include <stdbool.h>
#include "mode.h"
#include "timer.h"

typedef struct mode_midilearn_t {
  struct settings_t *settings;
  timer_t timer;
  bool leds_on;
  
} mode_midilearn_t;

void mode_midilearn_event(mode_t *cxt, enum event ev);

#endif /* MODE_MIDILEARN_H */
