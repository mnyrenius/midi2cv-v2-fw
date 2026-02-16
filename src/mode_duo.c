#include "mode_duo.h"
#include "notemem.h"
#include "constants.h"
#include "settings.h"
#include "dac.h"
#include "gate.h"
#include "led.h"

static void mode_init(mode_duo_t *cxt)
{
  notemem_init(cxt->notemem, NM_PRIO_LAST);
  cxt->current_base_note = 0xff;
}

static uint8_t is_for_me(uint8_t base_channel, uint8_t channel)
{
  return channel == base_channel;
}

static void mode_note_on(mode_duo_t *cxt, uint8_t note)
{
  if (note < NUM_NOTES) {
    if (cxt->current_base_note != 0xff) {
      uint8_t n = notemem_note_on(cxt->notemem, note);
      if (n < NUM_NOTES) {
        if (cxt->retrig) {
            gate_off(1);
        }

        dac_write(1, cxt->dac_values[n]);
        gate_on(1);
        led_on(1);
      }
    }
    else {
      cxt->current_base_note = note;
      dac_write(0, cxt->dac_values[note]);
      gate_on(0);
      led_on(0);
    }
  }
}

static void mode_note_off(mode_duo_t *cxt, uint8_t note)
{
  uint8_t next = notemem_note_off(cxt->notemem, note);
  if (next < NUM_NOTES) {
    dac_write(1, cxt->dac_values[next]);
  }
  else {
    gate_off(1);
    led_off(1);

    if (cxt->current_base_note != 0xff) {
      gate_off(0);
      led_off(0);
      cxt->current_base_note = 0xff;
    }
  }
}

void mode_duo_event(mode_t *cxt, enum event ev)
{
  switch (ev) {
    case EVENT_INIT:
      mode_init(cxt->duo_cxt);
      break;
    case EVENT_NOTE_ON:
      if (is_for_me(cxt->duo_cxt->settings->midi_channel, cxt->channel)) {
        mode_note_on(cxt->duo_cxt, cxt->note);
      }
      break;
    case EVENT_NOTE_OFF:
      if (is_for_me(cxt->duo_cxt->settings->midi_channel, cxt->channel)) {
        mode_note_off(cxt->duo_cxt, cxt->note);
      }
      break;
    default:
      break;
  };
}
