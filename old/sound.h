#ifndef _SOUND_H
#define _SOUND_H

#include <inttypes.h>


#define DEFAULT_TEMPO 120
#define DEFAULT_PATTERN_LENGTH 32

#define SAMPLE_FREQ 8000
#define BLOCK_LENGTH 16
#define BLOCK_FREQ SAMPLE_FREQ / BLOCK_LENGTH
#define BLOCK_CYCLES 48000000 / BLOCK_FREQ

#define ROWS_PER_BEAT 4


typedef struct sound_osc sound_osc_t;

typedef int8_t (*sound_osc_callback_t)(sound_osc_t *oscillator);

typedef struct sound_osc {
    sound_osc_callback_t handler;
    uint16_t phase;
    uint16_t inc;
} sound_osc_t;

// 64 bytes
typedef struct sound_pattern {
    uint8_t steps[64];
} sound_pattern_t;

// 65 bytes
typedef struct sound_proj_osc {
    uint8_t index;
    uint8_t arrangement[64];
} sound_proj_osc_t;

// 2440 bytes
typedef struct sound_proj_state {
    uint8_t tempo;
    uint8_t pattern_length;
    sound_pattern_t patterns[32];
    sound_proj_osc_t oscs[4];
} sound_proj_state_t;

extern sound_proj_state_t state;
int8_t sound_GenerateSample(void);
void sound_Setup(void);

#endif
