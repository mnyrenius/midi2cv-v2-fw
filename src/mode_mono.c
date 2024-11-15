#include "mode_mono.h"
#include "notemem.h"
#include "constants.h"
#include "settings.h"
#include "dac.h"
#include "gate.h"
#include "led.h"

#define CLOCK_PIN 4

static void mode_init(mode_mono_t *cxt)
{
  notemem_init(cxt->notemem, NM_PRIO_LAST);
  cxt->clock_counter = 0;
}

static uint8_t is_for_me(uint8_t base_channel, uint8_t channel)
{
  return (channel >= base_channel && channel < (base_channel + NUM_CHANNELS));
}

static void mode_note_on(mode_mono_t *cxt, uint8_t note, uint8_t channel)
{
  if (note < NUM_NOTES) {
    uint8_t n = notemem_note_on(cxt->notemem, note);
    if (n < NUM_NOTES) {
      if (cxt->retrig) {
        gate_off(channel);
      }

      dac_write(channel, cxt->dac_values[n]);
      gate_on(channel);
      led_on(channel);
    }
  }
}

static void mode_note_off(mode_mono_t *cxt, uint8_t note, uint8_t channel)
{
  uint8_t next = notemem_note_off(cxt->notemem, note);
  if (next < NUM_NOTES) {
    dac_write(channel, cxt->dac_values[next]);
  }
  else {
    gate_off(channel);
    led_off(channel);
  }
}

static void mode_clock(mode_mono_t *cxt)
{
  if (cxt->clock_running) {
    cxt->clock_counter++;
    if (cxt->clock_counter == 12)
    {
      gate_on(CLOCK_PIN);
      led_on(CLOCK_PIN);
      gate_off(CLOCK_PIN);
      led_off(CLOCK_PIN);
      cxt->clock_counter = 0;
    }
  }
}

static void mode_start(mode_mono_t *cxt)
{
  cxt->clock_running = 1;
}

static void mode_stop(mode_mono_t *cxt)
{
  cxt->clock_running = 0;
  cxt->clock_counter = 0;
}

void mode_mono_event(mode_t *cxt, enum event ev)
{
  switch (ev) {
    case EVENT_INIT:
      mode_init(cxt->mono_cxt);
      break;
    case EVENT_NOTE_ON:
      if (is_for_me(cxt->base_channel, cxt->channel)) {
        mode_note_on(cxt->mono_cxt, cxt->note, cxt->channel);
      }
      break;
    case EVENT_NOTE_OFF:
      if (is_for_me(cxt->base_channel, cxt->channel)) {
        mode_note_off(cxt->mono_cxt, cxt->note, cxt->channel);
      }
      break;
    case EVENT_RT_CLOCK:
      mode_clock(cxt->mono_cxt);
      break;
    case EVENT_RT_START:
      mode_start(cxt->mono_cxt);
      break;
    case EVENT_RT_STOP:
      mode_stop(cxt->mono_cxt);
      break;
    default:
      break;
  };
}
