#ifndef MODE_SHARE_H
#define MODE_SHARE_H

#include "mode.h"
#include "constants.h"

typedef struct mode_share_t {
  struct settings_t *settings;
  uint16_t *dac_values;
  uint8_t num_pressed;
  uint8_t notes[NUM_CHANNELS];
} mode_share_t;

void mode_share_event(mode_t *cxt, enum event ev);

#endif /* MODE_SHARE_H */
