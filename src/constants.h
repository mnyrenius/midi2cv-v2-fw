#ifndef CONSTANTS_H
#define CONSTANTS_H

#define NUM_CHANNELS   4
#define NUM_OCTAVES    10
#define NUM_NOTES      (12 * NUM_OCTAVES) + 1
#define NUM_DAC_VALUES 4096
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define UBRR_31250B 31

#define UBRR_CONF UBRR_31250B

typedef struct output_t {
  uint16_t cv[NUM_CHANNELS];
  uint8_t gates[NUM_CHANNELS];
  uint8_t leds[NUM_CHANNELS];
  uint8_t updated[NUM_CHANNELS];
} output_t;

#endif /* CONSTANTS_H */
