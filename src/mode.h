#ifndef MODE_H
#define MODE_H

#include <stdint.h>

enum event {
  EVENT_INIT,
  EVENT_NOTE_ON,
  EVENT_NOTE_OFF,
  EVENT_RT_CLOCK,
  EVENT_RT_START,
  EVENT_RT_STOP,
  EVENT_EXIT,

  EVENT_END,
};

enum mode {
  MODE_UNISON_LEGATO,
  MODE_UNISON_RETRIG,
  MODE_MIDI_LEARN,
  MODE_TURINGMACHINE,
  MODE_POLY_LEGATO,
  MODE_POLY_RETRIG,
  MODE_SHARE,
  MODE_MONO_LEGATO,
  MODE_MONO_RETRIG,
  MODE_MENU,
  MODE_END
};

typedef struct mode_t {
  void (*event)(struct mode_t*, enum event);
  uint8_t channel, note, num_channels;
  union {
    struct mode_unison_t *unison_cxt;
    struct mode_midilearn_t *midilearn_cxt;
    struct mode_turing_t *turing_cxt;
    struct mode_menu_t *menu_cxt;
    struct mode_poly_t *poly_cxt;
    struct mode_share_t *share_cxt;
    struct mode_mono_t *mono_cxt;
  };
} mode_t;

#endif /* MODE_H */

