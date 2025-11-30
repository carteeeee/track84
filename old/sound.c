#include "sound.h"
#include "data/tables.h"

#include <math.h>
#include <inttypes.h>
#include <stdio.h>

#define NUM_OSCS 4

sound_proj_state_t state = {
    .tempo = DEFAULT_TEMPO,
    .pattern_length = DEFAULT_PATTERN_LENGTH,
    .patterns = {
        {},
        {32, 32, 31, 29},
    },
    // everything else here should be initialized to 0
};

uint24_t samples_per_row = 0;

static int8_t osc_Sine(sound_osc_t *osc) {
    return sine_table[osc->phase >> 8];
}

static int8_t osc_Saw(sound_osc_t *osc) {
    return (int)(osc->phase >> 8) - 128;
}

// TODO: add pulse-width modulation maybe?
static int8_t osc_Square(sound_osc_t *osc) {
    return ((int)(osc->phase < 32768) * 255) - 128;
}

static int8_t osc_Triangle(sound_osc_t *osc) {
    return tri_table[osc->phase >> 8];
}

static int8_t osc_Noise(sound_osc_t *osc) {
    // TODO
    (void)osc;
    return 0;
}

static int8_t osc_Null(sound_osc_t *osc) {
    (void)osc;
    return 0;
}

static const sound_osc_callback_t osc_types[] = {osc_Sine, osc_Saw, osc_Square, osc_Triangle, osc_Noise, osc_Null};

static sound_osc_t oscillators[NUM_OSCS] = {
    {.handler = osc_Sine},
    {.handler = osc_Sine},
    {.handler = osc_Sine},
    {.handler = osc_Sine},
    {.handler = osc_Sine},
    {.handler = osc_Sine},
};

void sound_Setup(void) {
    samples_per_row = 60 * SAMPLE_FREQ / state.tempo / ROWS_PER_BEAT;

    for (uint8_t i = 0; i < NUM_OSCS; i++) {
        oscillators[i].handler = osc_types[state.oscs[i].index];
    }
}

int8_t sound_GenerateSample(void) {
    static uint24_t time = 0;
    static uint24_t lastrow = 0;
    static uint8_t row = 0;
    static uint8_t pattern = 0;
    int8_t sample = 0;
    time++;

    // this shit is somehow more efficient than actual divsion
    if (time == lastrow + samples_per_row) {
        row++;
        lastrow += samples_per_row;
    }

    if (row == state.pattern_length) {
        pattern++;
        row = 0;
    }

    for (uint8_t i = 0; i < NUM_OSCS; i++) {
        sound_osc_t *osc = &oscillators[i];
        //osc->inc = note_table[state.patterns[state.oscs[i].arrangement[pattern]].steps[row]];    
        //osc->inc = note_table[32];
        uint8_t pattern_index = *(&state.oscs[0].arrangement[0] + i * sizeof(state.oscs[0]) + pattern);
        osc->inc = note_table[
            state.patterns[
                pattern_index
            ].steps[row]
        ];
        sample += osc->handler(osc) >> 3;
        osc->phase += osc->inc;
    }

    return sample;
}
