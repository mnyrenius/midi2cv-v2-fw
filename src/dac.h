#ifndef DAC_H
#define DAC_H

#include <stdint.h>

void dac_init(void);
void dac_write(uint8_t channel, uint16_t value);

#endif /* DAC_H */
