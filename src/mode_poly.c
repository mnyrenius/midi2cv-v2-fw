#include "mode_poly.h"
#include "constants.h"
#include "settings.h"
#include "dac.h"
#include "gate.h"
#include "led.h"

static void mode_init(mode_poly_t *cxt)
{
  cxt->next_channel = 0;
}

static uint8_t is_for_me(uint8_t base_channel, uint8_t channel)
{
  return channel == base_channel;
}

static void mode_note_on(mode_poly_t *cxt, uint8_t note)
{
  if (note < NUM_NOTES) {
    if (cxt->next_channel < NUM_CHANNELS) {
      dac_write(cxt->next_channel, cxt->dac_values[note]);
      if (cxt->retrig) {
        for (uint8_t i = 0; i < cxt->next_channel; ++i) {
          gate_off(i);
        }
        for (uint8_t i = 0; i < cxt->next_channel; ++i) {
          gate_on(i);
        }
      }
      gate_on(cxt->next_channel);
      led_on(cxt->next_channel);
      cxt->next_channel++;
    }
  }
}

static void mode_note_off(mode_poly_t *cxt, uint8_t note)
{
  if (note < NUM_NOTES && cxt->next_channel > 0) {
    cxt->next_channel--;
    gate_off(cxt->next_channel);
    led_off(cxt->next_channel);
  }
}

void mode_poly_event(mode_t *cxt, enum event ev)
{
  switch (ev) {
    case EVENT_INIT:
      mode_init(cxt->poly_cxt);
      break;
    case EVENT_NOTE_ON:
      if (is_for_me(cxt->base_channel, cxt->channel)) {
        mode_note_on(cxt->poly_cxt, cxt->note);
      }
      break;
    case EVENT_NOTE_OFF:
      if (is_for_me(cxt->base_channel, cxt->channel)) {
        mode_note_off(cxt->poly_cxt, cxt->note);
      }
      break;
    default:
      break;
  };
}
