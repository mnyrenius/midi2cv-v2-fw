#include "mode_menu.h"
#include <avr/io.h>
#include "settings.h"
#include "led.h"
#include <util/delay.h>
#include "constants.h"
#include <string.h>

/* Settings menu

  Gate leds forms a binary number 1-15 that represent a mode (top led = MSB).

   1 - Unison, legato, note prio last
   2 - Unison, retrig, note prio last
   3 - Midi learn
   4 - Turing machine
   5 - Polyphonic, legato
   6 - Polyphonic, retrig
   7 - Unison/share, legato
  15 - Menu

*/

static void update_leds(uint8_t mode)
{
  mode++;

  uint8_t mask = 0x01;
  for (uint8_t i = 0; i < NUM_CHANNELS; ++i) {
    if (mode & mask)
      led_on(i);
    else
      led_off(i);
    mask <<= 1;
  }
}

static void mode_init(mode_menu_t *cxt)
{
  cxt->menu_index = cxt->settings->mode_prev;
  update_leds(cxt->menu_index);
}

static void mode_note_on(mode_menu_t *cxt, uint8_t note)
{
  switch (note % 12) {
    case 0:
      if (cxt->menu_index > 0)
        cxt->menu_index--;
      update_leds(cxt->menu_index);
      break;
    case 2:
      if (cxt->menu_index < 15)
        cxt->menu_index++;
      update_leds(cxt->menu_index);
      break;
    case 4:
      cxt->settings->mode = cxt->menu_index;
      settings_write(cxt->settings);
      __asm__("jmp 0"); // soft reset to reload settings
      break;
    default:
      break;
  }
}

void mode_menu_event(mode_t *cxt, enum event ev)
{
  switch (ev) {
    case EVENT_INIT:
      mode_init(cxt->menu_cxt);
      break;
    case EVENT_NOTE_ON:
      mode_note_on(cxt->menu_cxt, cxt->note);
      break;
    default:
      break;
  }
}
