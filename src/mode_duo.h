#ifndef MODE_DUO_H
#define MODE_DUO_H

#include "mode.h"

typedef struct mode_duo_t {
  struct settings_t *settings;
  struct notemem_t *notemem;
  uint16_t *dac_values;
  uint8_t retrig;
  uint8_t current_base_note;
} mode_duo_t;

void mode_duo_event(mode_t *cxt, enum event ev);

#endif /* MODE_DUO_H */
