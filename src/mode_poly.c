#include "mode_poly.h"
#include "notemem.h"
#include "constants.h"
#include "settings.h"
#include <string.h>

static void mode_init(mode_poly_t *cxt)
{
  notemem_init(cxt->notemem, NM_PRIO_LAST);
  cxt->next_channel = 0;
}

static void mode_note_on(mode_poly_t *cxt, uint8_t note)
{
  if (note < NUM_NOTES) {
    uint8_t n = notemem_note_on(cxt->notemem, note);
    if (n < NUM_NOTES) {
    }
  }
}

static void mode_note_off(mode_poly_t *cxt, uint8_t note)
{
  uint8_t next = notemem_note_off(cxt->notemem, note);
  if (next < NUM_NOTES) {
  }
  else {
    memset(cxt->out->gates, 0, NUM_CHANNELS);
    memset(cxt->out->leds, 0, NUM_CHANNELS);
  }

  memset(cxt->out->updated, 1, NUM_CHANNELS);
}

void mode_poly_event(mode_t *cxt, enum event ev)
{
  switch (ev) {
    case EVENT_INIT:
      mode_init(cxt->poly_cxt);
      break;
    case EVENT_NOTE_ON:
      mode_note_on(cxt->poly_cxt, cxt->note);
      break;
    case EVENT_NOTE_OFF:
      mode_note_off(cxt->poly_cxt, cxt->note);
      break;
    default:
      break;
  };
}
