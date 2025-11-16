#include "sound.h"
#include "data/tables.h"

#include <math.h>
#include <inttypes.h>
#include <stdio.h>

static int8_t osc_Sine(sound_osc_t *osc) {
    return sine_table[osc->phase >> 8];
}

static int8_t osc_Saw(sound_osc_t *osc) {
    return (int)(osc->phase >> 8) - 128;
}

// TODO: add pulse-width modulation somehow
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

static int8_t osc_Process(sound_osc_t *osc) {
    int8_t sample = osc->handler(osc);
    osc->phase += osc->inc;
    return sample;
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

void sound_SetOscs(int o0, int o1, int o2, int o3, int o4, int o5) {
    oscillators[0].handler = osc_types[o0];
    oscillators[1].handler = osc_types[o1];
    oscillators[2].handler = osc_types[o2];
    oscillators[3].handler = osc_types[o3];
    oscillators[4].handler = osc_types[o4];
    oscillators[5].handler = osc_types[o5];
}

void sound_SetPitches(int p0, int p1, int p2, int p3, int p4, int p5) {
    oscillators[0].inc = note_table[p0];
    oscillators[1].inc = note_table[p1];
    oscillators[2].inc = note_table[p2];
    oscillators[3].inc = note_table[p3];
    oscillators[4].inc = note_table[p4];
    oscillators[5].inc = note_table[p5];
}

int8_t sound_GenerateSample(void) {
    int8_t sample = 0;
    // the following is a really shitty event handler that is not currently in use
    /*static uint16_t time = 0;
    static uint8_t ev = 0;
    static uint32_t ev_time[] = {0, 1000, 2000, 3000, 4000, 10000,
                                 16000, 17000, 18000, 19000, 20000, 26000, 28000, 29000, 34000};
    static uint8_t ev_freq[] = {56, 58, 61, 58, 65, 63,
                                56, 58, 60, 58, 63, 61, 60, 58, 0};*/
    
    /*if (time >= ev_time[ev]) {
        if (ev_freq[ev] == 0) {
            ev = 0;
            time = 0;
        } else {
            inc = (880.f * pow(2, (ev_freq[ev]-69.f)/12.f)) * (65536.f / SAMPLE_FREQ);
            ev++;
        }
    }*/
    
    //time++;

    for (uint8_t i = 0; i < NUM_OSCS; i++) {
        sample += osc_Process(&oscillators[i]) >> 3;
    }

    return sample;
}
