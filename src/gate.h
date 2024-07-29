#ifndef GATE_H
#define GATE_H

#include <stdint.h>

void gate_init(void);
void gate_on(uint8_t index);
void gate_off(uint8_t index);

#endif /* GATE_H */