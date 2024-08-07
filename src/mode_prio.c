#include "mode_prio.h"
#include "notemem.h"
#include "constants.h"
#include "settings.h"
#include "dac.h"
#include "gate.h"
#include "led.h"

static void mode_init(mode_prio_t *cxt)
{
  notemem_init(cxt->notemem, NM_PRIO_LAST);
}

static void mode_note_on(mode_prio_t *cxt, uint8_t note)
{
  if (note < NUM_NOTES) {
    uint8_t n = notemem_note_on(cxt->notemem, note);
    if (n < NUM_NOTES) {
      if (cxt->retrig) {
        for (uint8_t i = 0; i < NUM_CHANNELS; ++i)
          gate_off(i);
      }
      for (uint8_t i = 0; i < NUM_CHANNELS; ++i) {
        dac_write(i, cxt->dac_values[n]);
        gate_on(i);
        led_on(i);
      }
    }
  }
}

static void mode_note_off(mode_prio_t *cxt, uint8_t note)
{
  uint8_t next = notemem_note_off(cxt->notemem, note);
  if (next < NUM_NOTES) {
    for (uint8_t i = 0; i < NUM_CHANNELS; ++i) {
      dac_write(i, cxt->dac_values[next]);
    }
  }
  else {
    for (uint8_t i = 0; i < NUM_CHANNELS; ++i) {
      gate_off(i);
      led_off(i);
    }
  }
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
