#include "mode_prio.h"
#include "notemem.h"
#include "constants.h"
#include "settings.h"
#include <string.h>

static void mode_init(mode_prio_t *cxt)
{
  notemem_init(cxt->notemem, NM_PRIO_LAST);
}

static void mode_note_on(mode_prio_t *cxt, uint8_t note)
{
  if (note < NUM_NOTES) {
    uint8_t n = notemem_note_on(cxt->notemem, note);
    if (n < NUM_NOTES) {
      cxt->out->cv[0] = cxt->dac_values[n];
      cxt->out->cv[1] = cxt->dac_values[n];
      cxt->out->cv[2] = cxt->dac_values[n];
      cxt->out->cv[3] = cxt->dac_values[n];
      memset(cxt->out->gates, 1, NUM_CHANNELS);
      memset(cxt->out->leds, 1, NUM_CHANNELS);
      memset(cxt->out->updated, 1, NUM_CHANNELS);
    }
  }
}

static void mode_note_off(mode_prio_t *cxt, uint8_t note)
{
  uint8_t next = notemem_note_off(cxt->notemem, note);
  if (next < NUM_NOTES) {
    cxt->out->cv[0] = cxt->dac_values[next];
    cxt->out->cv[1] = cxt->dac_values[next];
    cxt->out->cv[2] = cxt->dac_values[next];
    cxt->out->cv[3] = cxt->dac_values[next];
  }
  else {
    memset(cxt->out->gates, 0, NUM_CHANNELS);
    memset(cxt->out->leds, 0, NUM_CHANNELS);
  }

  memset(cxt->out->updated, 1, NUM_CHANNELS);
}

void mode_prio_event(mode_t *cxt, enum event ev)
{
  switch (ev) {
    case EVENT_INIT:
      mode_init(cxt->prio_cxt);
      break;
    case EVENT_NOTE_ON:
      mode_note_on(cxt->prio_cxt, cxt->note);
      break;
    case EVENT_NOTE_OFF:
      mode_note_off(cxt->prio_cxt, cxt->note);
      break;
    default:
      break;
  };
}
