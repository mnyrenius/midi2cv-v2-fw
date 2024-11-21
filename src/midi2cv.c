#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>
#include "dac.h"
#include "uart.h"
#include "gate.h"
#include "led.h"
#include "midi.h"
#include "settings.h"
#include "notemem.h"
#include "turing.h"
#include "mode.h"
#include "mode_unison.h"
#include "mode_midilearn.h"
#include "mode_turing.h"
#include "mode_menu.h"
#include "mode_poly.h"
#include "mode_share.h"
#include "mode_mono.h"
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

typedef struct menu_enter_t {
  uint8_t idx;
  uint8_t notes[3];
} menu_enter_t;

typedef struct midi2cv_t {
  settings_t settings;
  notemem_t notemem;
  turing_t turing;
  uint16_t dac_values[NUM_NOTES];
  mode_t modes[MODE_END];
  menu_enter_t menu_enter;
} midi2cv_t;

void generate_dac_values(uint16_t *values)
{
  for (uint16_t i = 0; i < NUM_NOTES; ++i)
  {
    values[i] = 4096 - 38 * i;
  }
}

void update_enter_menu(midi2cv_t *cxt, uint8_t note)
{
  menu_enter_t *p = &cxt->menu_enter;

  if (p->idx < 2) {
    p->notes[++p->idx] = note;
  }
}

bool should_enter_menu(midi2cv_t *cxt)
{
  menu_enter_t *p = &cxt->menu_enter;

  bool enter_menu = false;
  if (p->idx == 2) {
    // Enter menu if pressing c#, d# and a#
    if (p->notes[0] % 12 == 1 && p->notes[1] % 12 == 3 && p->notes[2] % 12 == 10) {
      enter_menu = true;
    }
  }

  return enter_menu;
}

bool is_midimsg_for_mode(uint8_t channel, uint8_t base_channel, uint8_t num_channels)
{
  return channel >= base_channel && channel < (base_channel + num_channels);
}

void note_on(void *arg, uint8_t channel, uint8_t note)
{
  midi2cv_t * cxt = (midi2cv_t *)arg;
  mode_t *m = &cxt->modes[cxt->settings.mode];

  update_enter_menu(cxt, note);

  if (should_enter_menu(cxt)) {
    cxt->settings.mode_prev = cxt->settings.mode;
    cxt->settings.mode = MODE_MENU;
    settings_write(&cxt->settings);
    __asm__("jmp 0"); // soft reset to reload settings
  } else if (is_midimsg_for_mode(channel, cxt->settings.midi_channel, m->num_channels)) {
      m->channel = channel;
      m->note = note > cxt->settings.midi_base_note ? note - cxt->settings.midi_base_note : 0;
      m->event(m, EVENT_NOTE_ON);
  }
}

void note_off(void *arg, uint8_t channel, uint8_t note)
{
  midi2cv_t * cxt = (midi2cv_t *)arg;

  if (cxt->menu_enter.idx > 0) {
    cxt->menu_enter.idx--;
  }

  mode_t *m = &cxt->modes[cxt->settings.mode];
  if (is_midimsg_for_mode(channel, cxt->settings.midi_channel, m->num_channels)) {
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

  mode_unison_t mode_unison_legato = {
    .settings = &midi2cv.settings,
    .notemem = &midi2cv.notemem,
    .dac_values = midi2cv.dac_values,
    .retrig = 0,
  };

  mode_unison_t mode_unison_retrig = {
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

  mode_mono_t mode_mono_legato = {
    .settings = &midi2cv.settings,
    .notemem = &midi2cv.notemem,
    .dac_values = midi2cv.dac_values,
    .retrig = 0,
  };

  mode_mono_t mode_mono_retrig = {
    .settings = &midi2cv.settings,
    .notemem = &midi2cv.notemem,
    .dac_values = midi2cv.dac_values,
    .retrig = 1,
  };

  midi2cv.modes[MODE_UNISON_LEGATO]  = (mode_t) { .event = mode_unison_event     , .unison_cxt    = &mode_unison_legato, .num_channels = 1 };
  midi2cv.modes[MODE_UNISON_RETRIG]  = (mode_t) { .event = mode_unison_event     , .unison_cxt    = &mode_unison_retrig, .num_channels = 1 };
  midi2cv.modes[MODE_MIDI_LEARN]     = (mode_t) { .event = mode_midilearn_event  , .midilearn_cxt = &mode_midilearn,     .num_channels = 1 };
  midi2cv.modes[MODE_TURINGMACHINE]  = (mode_t) { .event = mode_turing_event     , .turing_cxt    = &mode_turing,        .num_channels = 1 };
  midi2cv.modes[MODE_POLY_LEGATO]    = (mode_t) { .event = mode_poly_event       , .poly_cxt      = &mode_poly_legato,   .num_channels = 1 };
  midi2cv.modes[MODE_POLY_RETRIG]    = (mode_t) { .event = mode_poly_event       , .poly_cxt      = &mode_poly_retrig,   .num_channels = 1 };
  midi2cv.modes[MODE_SHARE]          = (mode_t) { .event = mode_share_event      , .share_cxt     = &mode_share,         .num_channels = 1 };
  midi2cv.modes[MODE_MONO_LEGATO]    = (mode_t) { .event = mode_mono_event       , .mono_cxt      = &mode_mono_legato,   .num_channels = 4 };
  midi2cv.modes[MODE_MONO_RETRIG]    = (mode_t) { .event = mode_mono_event       , .mono_cxt      = &mode_mono_retrig,   .num_channels = 4 };
  midi2cv.modes[MODE_MENU]           = (mode_t) { .event = mode_menu_event       , .menu_cxt      = &mode_menu,          .num_channels = 1 };

  generate_dac_values(midi2cv.dac_values);
  settings_read(&midi2cv.settings);
  midi2cv.menu_enter.idx = 0;

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
