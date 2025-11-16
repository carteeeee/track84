#ifndef _SOUND_H
#define _SOUND_H

#include <inttypes.h>

#define SAMPLE_FREQ 8000
#define BLOCK_LENGTH 16
#define BLOCK_FREQ SAMPLE_FREQ / BLOCK_LENGTH
#define BLOCK_CYCLES 48000000 / BLOCK_FREQ

#define NUM_OSCS 6

typedef struct sound_osc sound_osc_t;

typedef int8_t (*sound_osc_callback_t)(sound_osc_t *oscillator);

typedef struct sound_osc {
    sound_osc_callback_t handler;
    uint16_t phase;
    uint16_t inc;
} sound_osc_t;

void sound_SetOscs(int o0, int o1, int o2, int o3, int o4, int o5);
void sound_SetPitches(int p0, int p1, int p2, int p3, int p4, int p5);
int8_t sound_GenerateSample(void);

#endif
