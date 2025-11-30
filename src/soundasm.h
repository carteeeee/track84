#ifndef _SOUNDASM_H
#define _SOUNDASM_H

#include <inttypes.h>

#define SAMPLE_FREQ 8000
#define BLOCK_LENGTH 16
#define BLOCK_FREQ SAMPLE_FREQ / BLOCK_LENGTH
#define BLOCK_CYCLES 48000000 / BLOCK_FREQ

#define MAX_PATTERN_LENGTH 64
#define MAX_SONG_LENGTH 64
#define NUM_PATTERNS 32
#define NUM_OSCS 6

// 64 bytes
typedef struct soundasm_pattern {
    uint8_t steps[MAX_PATTERN_LENGTH];
} soundasm_pattern_t;

// 65 bytes
typedef struct soundasm_proj_osc {
    uint8_t index;
    uint8_t arrangement[MAX_SONG_LENGTH];
} soundasm_proj_osc_t;

// 2440 bytes
typedef struct soundasm_proj_state {
    uint8_t tempo;
    uint8_t pattern_length;
    soundasm_proj_osc_t oscs[NUM_OSCS];
    soundasm_pattern_t patterns[NUM_PATTERNS];
} soundasm_proj_state_t;

extern soundasm_proj_state_t soundasm_state;
void soundasm_Setup(void);
int8_t soundasm_GenerateSample(void);

#endif
