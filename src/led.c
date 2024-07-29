#include "led.h"
#include <avr/io.h>

void led_init(void)
{
  DDRD |= _BV (PD1) | _BV (PD2) | _BV (PD3) | _BV (PD4) | _BV (PD5);
  PORTD &= ~ 0x3e;
}

void led_on(uint8_t index)
{
  uint8_t pin = 0;
  switch (index)
  {
  case 0:
    pin = PD4;
    break;
  case 1:
    pin = PD2;
    break;
  case 2:
    pin = PD3;
    break;
  case 3:
    pin = PD5;
    break;
  case 4:
    pin = PD1;
    break;
  default:
    break;
  }

  PORTD |= _BV(pin);
}

void led_off(uint8_t index)
{
  uint8_t pin = 0;
  switch (index)
  {
  case 0:
    pin = PD4;
    break;
  case 1:
    pin = PD2;
    break;
  case 2:
    pin = PD3;
    break;
  case 3:
    pin = PD5;
    break;
  case 4:
    pin = PD1;
    break;
  default:
    break;
  }

  PORTD &= ~_BV(pin);
}