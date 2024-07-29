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

typedef struct hades_t {
  settings_t settings;
  notemem_t notemem;
  turing_t turing;
  uint16_t dac_values[NUM_NOTES];
  mode_t modes[MODE_END];
  output_t out;
} hades_t;

void generate_dac_values(uint16_t *values)
{
  // v_out = v_ref * d / 4096
  // 1 volt is = 819 which gives 819 / 12 = 68.25 per
  // semi note. since we need an integer, multiply by 4 -> 273 and
  // use this as the step between each note.
  // finally divide each note value by 4 to get the real dac value again and it
  // should hopefully be quite close to 1v/oct.
  //for (uint8_t i = 0; i < NUM_NOTES; ++i)
  //  values[i] = MIN(NUM_DAC_VALUES - 1, 273 * i / 4);

  for (uint16_t i = 0; i < NUM_NOTES; ++i)
  {
    values[i] = 4096 - 38 * i;
  }
}

void note_on(void *arg, uint8_t channel, uint8_t note)
{
  hades_t * cxt = (hades_t *)arg;
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
  hades_t * cxt = (hades_t *)arg;
  if (channel == cxt->settings.midi_channel) {
    mode_t *m = &cxt->modes[cxt->settings.mode];
    m->channel = channel;
    m->note = note > cxt->settings.midi_base_note ? note - cxt->settings.midi_base_note : 0;
    m->event(m, EVENT_NOTE_OFF);
  }
}

void clock(void *arg)
{
  hades_t * cxt = (hades_t *)arg;
  mode_t *m = &cxt->modes[cxt->settings.mode];
  m->event(m, EVENT_RT_CLOCK);
}

void start(void *arg)
{
  hades_t * cxt = (hades_t *)arg;
  mode_t *m = &cxt->modes[cxt->settings.mode];
  m->event(m, EVENT_RT_START);
}

void stop(void *arg)
{
  hades_t * cxt = (hades_t *)arg;
  mode_t *m = &cxt->modes[cxt->settings.mode];
  m->event(m, EVENT_RT_STOP);
}

int main()
{
  dac_init();
  gate_init();
  led_init();

  hades_t hades = {
    .out = {
      .cv = {0},
      .gates = {0},
      .leds = {0},
      .updated = {0},
    },
  };

  mode_prio_t mode_prio = {
    .settings = &hades.settings,
    .notemem = &hades.notemem,
    .dac_values = hades.dac_values,
    .out = &hades.out,
  };

  mode_midilearn_t mode_midilearn = {
    .settings = &hades.settings,
  };


  mode_turing_t mode_turing = {
    .settings = &hades.settings,
    .turing = &hades.turing,
    .dac_values = hades.dac_values,
    .out = &hades.out,
  };

  mode_menu_t mode_menu = {
    .settings = &hades.settings,
    .out = &hades.out,
  };

  hades.modes[MODE_UNISON_LEGATO]  = (mode_t) { .event = mode_prio_event       , .prio_cxt      = &mode_prio      };
  hades.modes[MODE_MIDI_LEARN]     = (mode_t) { .event = mode_midilearn_event  , .midilearn_cxt = &mode_midilearn };
  hades.modes[MODE_TURINGMACHINE]  = (mode_t) { .event = mode_turing_event     , .turing_cxt    = &mode_turing    };
  hades.modes[MODE_MENU]           = (mode_t) { .event = mode_menu_event       , .menu_cxt      = &mode_menu      };

  generate_dac_values(hades.dac_values);
  settings_read(&hades.settings);

  mode_t *m = &hades.modes[hades.settings.mode];
  m->event(m, EVENT_INIT);

  midi_t midi = {
    .callback_data = &hades,
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
    m = &hades.modes[hades.settings.mode];
    m->event(m, EVENT_UPDATE);

    for (uint8_t i = 0; i < NUM_CHANNELS; ++i) {
      if (hades.out.updated[i]) {
        dac_write(0, hades.out.cv[i]);

        if (hades.out.gates[i]) {
          gate_on(i);
        }
        else {
          gate_off(i);
        }

        if (hades.out.leds[i]) {
          led_on(i);
        }
        else{
          led_off(i);
        }

        hades.out.updated[i] = 0;
      }
    }
  }
}
