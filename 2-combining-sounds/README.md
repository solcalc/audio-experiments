# combining-sounds

Synthesizing notes and combining them into chords — and exploring what happens when you sum, mix, and clip multiple waveforms together.

Building on `1-learning-wav`, this experiment stops writing single sine tones and starts stacking them. When you add several sine waves the samples can exceed the `int16_t` range, so the interesting question becomes: how do you combine voices without ugly digital clipping?

## Build

```
gcc -o main main.c -lm
```

## Run

```
./main
aplay out.wav
```

## Ideas to explore

- **Additive mixing** — sum the samples of two or more notes to build a chord (e.g. C–E–G major triad).
- **Normalization vs. clipping** — naive summing overflows; compare hard clipping against scaling the mix down by `1/n`.
- **Soft clipping / saturation** — run the summed signal through `tanh(k * x)` and vary `k` to control the "corner sharpness" near ±1. Small `k` stays clean and linear; large `k` squares off the peaks and adds harmonics (analog-style warmth/overdrive).
- **Weighted mixes** — give voices different amplitudes so the chord has a root that dominates.
- **Detuning & beating** — offset two notes by a few Hz and hear the amplitude beating.

## Soft-clipping sketch

```c
// k controls the corner sharpness near the -1 / +1 rails
float mix = (a + b + c) / 3.0f;      // average the voices
float shaped = tanhf(k * mix);        // saturate
sample = (int16_t)(shaped * 32767);
```

```c
static inline float softclamp_ref(float x, float n) {
    float ax = fabsf(x);
    if (ax == 0.0f) return 0.0f;
    // Protect against extreme n / overflow if you want
    float p = powf(ax, n);
    return x * powf(1.0f + p, -1.0f / n);   // mul + reciprocal-pow is often slightly better
}
```

```c
void softclamp_lut_init(SoftClampLUT *s, float n) {
    s->n     = n;
    s->xmax  = SOFTCLAMP_X_MAX;
    s->scale = SOFTCLAMP_SCALE;

    for (int i = 0; i < SOFTCLAMP_LUT_SIZE; ++i) {
        float x = (float)i / (float)(SOFTCLAMP_LUT_SIZE - 1) * s->xmax;
        s->lut[i] = softclamp_ref(x, n);
    }
    s->lut[0] = 0.0f;                       // exact
    // Optional: force last entry exactly 1 if you prefer hard clamp beyond xmax
    // s->lut[SOFTCLAMP_LUT_SIZE-1] = 1.0f;
}

// The fast path — this is what you call in the hot loop
static inline float softclamp_lut(const SoftClampLUT *s, float x) {
    float ax = fabsf(x);

    // Early out for the flat region (branch predicts extremely well)
    if (ax >= s->xmax) {
        return copysignf(1.0f, x);
    }

    // Map [0, xmax] → [0, SIZE-1]
    float fidx = ax * s->scale;
    int   idx  = (int)fidx;                 // trunc toward zero == floor for positive
    float frac = fidx - (float)idx;

    // Linear interpolation (one fma if the compiler is smart)
    float y = s->lut[idx] + frac * (s->lut[idx + 1] - s->lut[idx]);

    return copysignf(y, x);
}
```
