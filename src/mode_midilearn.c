#include "mode_midilearn.h"
#include "settings.h"
#include "led.h"

static void timer_callback(void *data)
{
  mode_midilearn_t *cxt = (mode_midilearn_t *)data;
  if (cxt->leds_on) {
    led_all_on();
    cxt->leds_on = false;
  }
  else {
    led_all_off();
    cxt->leds_on = true;
  }
}

static void mode_init(mode_midilearn_t *cxt)
{
  cxt->leds_on = true;
  cxt->timer.period_ms = 250;
  cxt->timer.timer_callback = timer_callback;
  cxt->timer.callback_data = cxt;
  timer_start(&cxt->timer);
}

static void mode_note_on(mode_midilearn_t *cxt, uint8_t channel, uint8_t note)
{
  if (channel < 15) {
    cxt->settings->midi_channel = channel;
    cxt->settings->midi_base_note = note;
    cxt->settings->mode = MODE_MONO; // Jump to, let's call it, default mode
    cxt->settings->retrig = 0;
    settings_write(cxt->settings);
  }
}

void mode_midilearn_event(mode_t *cxt, enum event ev)
{
  switch (ev) {
    case EVENT_INIT:
      mode_init(cxt->midilearn_cxt);
      break;
    case EVENT_NOTE_ON:
      mode_note_on(cxt->midilearn_cxt, cxt->channel, cxt->note);
      break;
    default:
      break;
  }
}
