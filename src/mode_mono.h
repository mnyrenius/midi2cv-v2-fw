#ifndef MODE_MONO_H
#define MODE_MONO_H

#include "mode.h"

typedef struct mode_mono_t {
  struct settings_t *settings;
  struct notemem_t *notemem;
  uint16_t *dac_values;
  uint8_t retrig;
  uint8_t clock_counter;
  uint8_t clock_running;
} mode_mono_t;

void mode_mono_event(mode_t *cxt, enum event ev);

#endif /* MODE_MONO_H */
