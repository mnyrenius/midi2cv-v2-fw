#include "mode_unison.h"
#include "notemem.h"
#include "constants.h"
#include "settings.h"
#include "dac.h"
#include "gate.h"
#include "led.h"

static void mode_init(mode_unison_t *cxt)
{
  notemem_init(cxt->notemem, NM_PRIO_LAST);
}

static uint8_t is_for_me(uint8_t base_channel, uint8_t channel)
{
  return channel == base_channel;
}

static void mode_note_on(mode_unison_t *cxt, uint8_t note)
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

static void mode_note_off(mode_unison_t *cxt, uint8_t note)
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

void mode_unison_event(mode_t *cxt, enum event ev)
{
  switch (ev) {
    case EVENT_INIT:
      mode_init(cxt->unison_cxt);
      break;
    case EVENT_NOTE_ON:
      if (is_for_me(cxt->base_channel, cxt->channel)) {
        mode_note_on(cxt->unison_cxt, cxt->note);
      }
      break;
    case EVENT_NOTE_OFF:
      if (is_for_me(cxt->base_channel, cxt->channel)) {
        mode_note_off(cxt->unison_cxt, cxt->note);
      }
      break;
    default:
      break;
  };
}
