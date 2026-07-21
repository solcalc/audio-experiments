#include <math.h>
#include <stdint.h>
#include "clamps.h"

// Definitions (single copy) for the tables declared extern in clamps.h
float lut_tanh[LUT_SIZE];
float lut_hardclip[LUT_SIZE];
float lut_rational1[LUT_SIZE];
float lut_rational2[LUT_SIZE];
float lut_rational5[LUT_SIZE];
float lut_rational10[LUT_SIZE];

void init_lut_functions() {
    build_lut(lut_tanh, shape_tanh, 1.0f);
    build_lut(lut_hardclip, shape_hardclip, 0.0f);
    build_lut(lut_rational1, shape_rational, 1.0f);
    build_lut(lut_rational2, shape_rational, 2.0f);
    build_lut(lut_rational5, shape_rational, 5.0f);
    build_lut(lut_rational10, shape_rational, 10.0f);
}

void build_lut(float *lut_table, shaper_fn in_fn, float param) {
    for (int i = 0; i < LUT_SIZE; i++) {
        float x = ((float)i / (LUT_SIZE - 1)) * 2.0f * LUT_INPUT_MAX - LUT_INPUT_MAX;
        lut_table[i] = in_fn(x, param);
    }
}

// ---- Generic LUT lookup w/ linear interpolation (same for all clippers) ----
float lut_lookup(float *lut, float x) {
    if (x <= -LUT_INPUT_MAX) return lut[0];
    if (x >= LUT_INPUT_MAX) return lut[LUT_SIZE - 1];

    float index = (x + LUT_INPUT_MAX) / (2.0f * LUT_INPUT_MAX) * (LUT_SIZE - 1);
    int index_low = (int)index;
    int index_high = index_low + 1;

    if (index_high >= LUT_SIZE) return lut[LUT_SIZE - 1];

    float frac = index - (float)index_low;
    return lut[index_low] * (1.0f - frac) + lut[index_high] * frac;
}

float shape_tanh(float x, float k) {
    if (k <= 0.0f) k = 1.0f;
    return tanhf(x); // normalize edge to ~1 at x = LUT_INPUT_MAX
}

float shape_hardclip(float x, float param) {
    (void)param;
    if (x >  1.0f) return  1.0f;
    if (x < -1.0f) return -1.0f;
    return x;
}

float shape_rational(float x, float n) {
    if (n <= 0.0f) n = 2.0f;
    float denom = powf(1.0f + powf(fabsf(x), n), 1.0f / n);
    return x / denom;
}
