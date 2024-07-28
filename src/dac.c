#include "dac.h"
#include <avr/io.h>

#define NUM_CHANNELS 4

void dac_init(void)
{
  // Setup slave selects, sck and mosi
  DDRB |= _BV(PB1) | _BV(PB2) | _BV(PB3) | _BV(PB5);

  // Enable spi, master mode, fcpu / 64
  SPSR = 0;
  SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1);

  for (uint8_t ch = 0; ch < NUM_CHANNELS; ++ch)
  {
    dac_write(ch, 0);
  }
}

void dac_write(uint8_t channel, uint16_t value)
{
  uint8_t config = 0;
  uint8_t port = 0;

  switch (channel)
  {
  case 0:
    config = 0x10;
    port = PB1;
    break;
  case 1:
    config = 0x90;
    port = PB1;
    break;
  case 2:
    config = 0x10;
    port = PB2;
    break;
  case 3:
    config = 0x90;
    port = PB2;
    break;
  default:
    break;
  }

  PORTB &= ~_BV(port);

  SPDR = (config & 0xf0) | (0x0f & (value >> 8));
  while (!(SPSR & (1 << SPIF)))
    ;

  SPDR = value & 0x00ff;
  while (!(SPSR & (1 << SPIF)))
    ;

  PORTB |= _BV(port);
}
