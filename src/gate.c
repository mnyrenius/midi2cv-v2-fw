#include "gate.h"
#include <avr/io.h>

void gate_init(void)
{
  DDRC  |= _BV (PC0) | _BV (PC1) | _BV (PC2) | _BV (PC3) | _BV (PC4);
  PORTC |= _BV (PC0) | _BV (PC1) | _BV (PC2) | _BV (PC3) | _BV (PC4);
}

void gate_on(uint8_t index)
{
  uint8_t pin = 0;
  switch (index)
  {
  case 0:
    pin = PC0;
    break;
  case 1:
    pin = PC1;
    break;
  case 2:
    pin = PC2;
    break;
  case 3:
    pin = PC3;
    break;
  case 4:
    pin = PC4;
    break;
  default:
    break;
  }

  PORTC &= ~_BV(pin);
}

void gate_off(uint8_t index)
{
  uint8_t pin = 0;
  switch (index)
  {
  case 0:
    pin = PC0;
    break;
  case 1:
    pin = PC1;
    break;
  case 2:
    pin = PC2;
    break;
  case 3:
    pin = PC3;
    break;
  case 4:
    pin = PC4;
    break;
  default:
    break;
  }

  PORTC |= _BV(pin);
}