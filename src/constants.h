#ifndef CONSTANTS_H
#define CONSTANTS_H

#define NUM_CHANNELS   4
#define NUM_OCTAVES    10
#define NUM_NOTES      (12 * NUM_OCTAVES) + 1
#define NUM_DAC_VALUES 4096
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define UBRR_31250B 31

#define UBRR_CONF UBRR_31250B

#endif /* CONSTANTS_H */
