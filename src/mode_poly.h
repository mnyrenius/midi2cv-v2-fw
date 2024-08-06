#ifndef MODE_POLY_H
#define MODE_POLY_H

#include "mode.h"

typedef struct mode_poly_t {
  struct settings_t *settings;
  struct notemem_t *notemem;
  uint16_t *dac_values;
  struct output_t *out;
  uint8_t next_channel;
} mode_poly_t;

void mode_poly_event(mode_t *cxt, enum event ev);

#endif /* MODE_POLY_H */
