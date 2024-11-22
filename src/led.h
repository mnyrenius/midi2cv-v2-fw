#ifndef LED_H
#define LED_H

#include <stdint.h>

void led_init(void);
void led_on(uint8_t index);
void led_off(uint8_t index);
void led_all_off(void);

#endif /* LED_H */