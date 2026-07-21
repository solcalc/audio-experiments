#ifndef CLAMPS_H
#define CLAMPS_H

#define LUT_SIZE      1024
#define LUT_INPUT_MAX 4.0f   // input range the table covers: [-LUT_INPUT_MAX, +LUT_INPUT_MAX]

static float lut_tanh[LUT_SIZE];
static float lut_hardclip[LUT_SIZE];
static float lut_rational1[LUT_SIZE];
static float lut_rational2[LUT_SIZE];
static float lut_rational5[LUT_SIZE];
static float lut_rational10[LUT_SIZE];

// ---- Generic LUT builder: pass in any shaping function ----
typedef float (*shaper_fn)(float x, float param);

void init_lut_functions();
void build_lut(float *lut_table, shaper_fn in_fn, float param);
float lut_lookup(float *lut, float x);

float shape_tanh(float x, float k);
float shape_hardclip(float x, float param);
float shape_rational(float x, float n);

#endif // CLAMPS_H
