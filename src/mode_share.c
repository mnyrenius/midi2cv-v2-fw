#include "mode_share.h"
#include "constants.h"
#include "settings.h"
#include "dac.h"
#include "gate.h"
#include "led.h"

static void mode_init(mode_share_t *cxt)
{
  cxt->num_pressed = 0;
  for (uint8_t i = 0; i < NUM_CHANNELS; ++i) {
    cxt->notes[i] = 0;
  }
}

static void mode_note_on(mode_share_t *cxt, uint8_t note)
{
  if (note < NUM_NOTES) {
    cxt->num_pressed++;
    switch (cxt->num_pressed) {
      case 1:
        cxt->notes[0] = note;
        cxt->notes[1] = note;
        cxt->notes[2] = note;
        cxt->notes[3] = note;
        break;
      case 2:
        cxt->notes[2] = note;
        cxt->notes[3] = note;
        break;
      case 3:
        cxt->notes[1] = note;
        break;
      case 4:
      cxt->notes[3] = note;
        break;
      default:
        break;
    }
    for (uint8_t i = 0; i < NUM_CHANNELS; ++i) {
      dac_write(i, cxt->dac_values[cxt->notes[i]]);
      gate_on(i);
      led_on(i);
    }
  }
}

static void mode_note_off(mode_share_t *cxt, uint8_t note)
{
  if (note < NUM_NOTES) {
    cxt->num_pressed--;
    switch (cxt->num_pressed) {
      case 1:
        cxt->notes[1] = cxt->notes[0];
        cxt->notes[2] = cxt->notes[0];
        cxt->notes[3] = cxt->notes[0];
        break;
      case 2:
        cxt->notes[2] = cxt->notes[1];
        cxt->notes[3] = cxt->notes[1];
        cxt->notes[1] = cxt->notes[0];
        break;
      case 3:
        cxt->notes[3] = cxt->notes[2];
        break;
      default:
        break;
    }
    for (uint8_t i = 0; i < NUM_CHANNELS; ++i) {
      if (cxt->num_pressed == 0) {
        gate_off(i);
        led_off(i);
      }
      dac_write(i, cxt->dac_values[cxt->notes[i]]);
    }
  }
}

void mode_share_event(mode_t *cxt, enum event ev)
{
  switch (ev) {
    case EVENT_INIT:
      mode_init(cxt->share_cxt);
      break;
    case EVENT_NOTE_ON:
      mode_note_on(cxt->share_cxt, cxt->note);
      break;
    case EVENT_NOTE_OFF:
      mode_note_off(cxt->share_cxt, cxt->note);
      break;
    default:
      break;
  };
}
