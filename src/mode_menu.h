#ifndef MODE_MENU_H
#define MODE_MENU_H

#include "mode.h"

typedef struct mode_menu_t {
  struct settings_t *settings;
  uint8_t menu_index;
  struct output_t *out;
} mode_menu_t;

void mode_menu_event(mode_t *cxt, enum event ev);

#endif /* MODE_MENU_H */
