#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include "dac.h"
#include "uart.h"
#include "gate.h"
#include "led.h"
#include "midi.h"
#include "settings.h"
#include "notemem.h"
#include "turing.h"
#include "mode.h"
#include "mode_prio.h"
#include "mode_midilearn.h"
#include "mode_turing.h"
#include "mode_menu.h"
#include "mode_poly.h"
#include "mode_share.h"
#include "constants.h"

/* ---------- PIN CONFIGURATION ----------
 *
 * PORTB
 *  1 - mcp4922 1 slave select
 *  2 - mcp4922 2 slave select
 *  5 - mcp4922 sck
 *  3 - mcp4922 mosi
 *
 * PORTC
 *  0 - gate channel 1
 *  1 - gate channel 2
 *  2 - gate channel 3
 *  3 - gate channel 4
 *  4 - clock 1
 *
 * PORTD
 *  0 - midi in
 *  1 - led D2
 *  2 - led D3
 *  3 - led D4
 *  4 - led D5
 *  5 - led D6
 *
 * ------------------------------------ */


/* --------- CV OUTPUT STAGE NOTES ---------
 *
 * V_out = - V_dac (R1/R2) + V_ref ((R1+R2)/R1)
 *
 * R1 = 100k
 * R2 = 180k
 *
 * ------------------------------------ */

typedef struct midi2cv_t {
  settings_t settings;
  notemem_t notemem;
  turing_t turing;
  uint16_t dac_values[NUM_NOTES];
  mode_t modes[MODE_END];
} midi2cv_t;

void generate_dac_values(uint16_t *values)
{
  for (uint16_t i = 0; i < NUM_NOTES; ++i)
  {
    values[i] = 4096 - 38 * i;
  }
}

void note_on(void *arg, uint8_t channel, uint8_t note)
{
  midi2cv_t * cxt = (midi2cv_t *)arg;
  mode_t *m = &cxt->modes[cxt->settings.mode];

  if (channel == 15) {
    cxt->settings.mode = MODE_MENU;
    settings_write(&cxt->settings);
    __asm__("jmp 0"); // soft reset to reload settings
  } else if (channel == cxt->settings.midi_channel || cxt->settings.mode == MODE_MIDI_LEARN) {
    m->channel = channel;
    m->note = note > cxt->settings.midi_base_note ? note - cxt->settings.midi_base_note : 0;
    m->event(m, EVENT_NOTE_ON);
  }
}

void note_off(void *arg, uint8_t channel, uint8_t note)
{
  midi2cv_t * cxt = (midi2cv_t *)arg;
  if (channel == cxt->settings.midi_channel) {
    mode_t *m = &cxt->modes[cxt->settings.mode];
    m->channel = channel;
    m->note = note > cxt->settings.midi_base_note ? note - cxt->settings.midi_base_note : 0;
    m->event(m, EVENT_NOTE_OFF);
  }
}

void clock(void *arg)
{
  midi2cv_t * cxt = (midi2cv_t *)arg;
  mode_t *m = &cxt->modes[cxt->settings.mode];
  m->event(m, EVENT_RT_CLOCK);
}

void start(void *arg)
{
  midi2cv_t * cxt = (midi2cv_t *)arg;
  mode_t *m = &cxt->modes[cxt->settings.mode];
  m->event(m, EVENT_RT_START);
}

void stop(void *arg)
{
  midi2cv_t * cxt = (midi2cv_t *)arg;
  mode_t *m = &cxt->modes[cxt->settings.mode];
  m->event(m, EVENT_RT_STOP);
}

int main()
{
  dac_init();
  gate_init();
  led_init();

  midi2cv_t midi2cv;

  mode_prio_t mode_prio_legato = {
    .settings = &midi2cv.settings,
    .notemem = &midi2cv.notemem,
    .dac_values = midi2cv.dac_values,
    .retrig = 0,
  };

  mode_prio_t mode_prio_retrig = {
    .settings = &midi2cv.settings,
    .notemem = &midi2cv.notemem,
    .dac_values = midi2cv.dac_values,
    .retrig = 1,
  };

  mode_midilearn_t mode_midilearn = {
    .settings = &midi2cv.settings,
  };


  mode_turing_t mode_turing = {
    .settings = &midi2cv.settings,
    .turing = &midi2cv.turing,
    .dac_values = midi2cv.dac_values,
  };

  mode_menu_t mode_menu = {
    .settings = &midi2cv.settings,
  };

  mode_poly_t mode_poly_legato = {
    .settings = &midi2cv.settings,
    .dac_values = midi2cv.dac_values,
    .retrig  = 0,
  };

  mode_poly_t mode_poly_retrig = {
    .settings = &midi2cv.settings,
    .dac_values = midi2cv.dac_values,
    .retrig  = 1,
  };

  mode_share_t mode_share = {
    .settings = &midi2cv.settings,
    .dac_values = midi2cv.dac_values,
  };


  midi2cv.modes[MODE_UNISON_LEGATO]  = (mode_t) { .event = mode_prio_event       , .prio_cxt      = &mode_prio_legato };
  midi2cv.modes[MODE_UNISON_RETRIG]  = (mode_t) { .event = mode_prio_event       , .prio_cxt      = &mode_prio_retrig };
  midi2cv.modes[MODE_MIDI_LEARN]     = (mode_t) { .event = mode_midilearn_event  , .midilearn_cxt = &mode_midilearn   };
  midi2cv.modes[MODE_TURINGMACHINE]  = (mode_t) { .event = mode_turing_event     , .turing_cxt    = &mode_turing      };
  midi2cv.modes[MODE_POLY_LEGATO]    = (mode_t) { .event = mode_poly_event       , .poly_cxt      = &mode_poly_legato };
  midi2cv.modes[MODE_POLY_RETRIG]    = (mode_t) { .event = mode_poly_event       , .poly_cxt      = &mode_poly_retrig };
  midi2cv.modes[MODE_SHARE]          = (mode_t) { .event = mode_share_event      , .share_cxt     = &mode_share       };
  midi2cv.modes[MODE_MENU]           = (mode_t) { .event = mode_menu_event       , .menu_cxt      = &mode_menu        };

  generate_dac_values(midi2cv.dac_values);
  settings_read(&midi2cv.settings);

  mode_t *m = &midi2cv.modes[midi2cv.settings.mode];
  m->event(m, EVENT_INIT);

  midi_t midi = {
    .callback_data = &midi2cv,
    .note_on = note_on,
    .note_off = note_off,
    .rt_clock = clock,
    .rt_start = start,
    .rt_stop = stop,
  };

  uint8_t rxb;

  midi_init(&midi);
  uart_init();

  sei();

  while (1) {
    if (uart_receive(&rxb) == 0)
      midi_process(&midi, rxb);
  }
}
