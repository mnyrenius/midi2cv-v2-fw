#ifndef MODE_UNISON_H
#define MODE_UNISON_H

#include "mode.h"

typedef struct mode_unison_t {
  struct settings_t *settings;
  struct notemem_t *notemem;
  uint16_t *dac_values;
  uint8_t retrig;
} mode_unison_t;

void mode_unison_event(mode_t *cxt, enum event ev);

#endif /* MODE_UNISON_H */
