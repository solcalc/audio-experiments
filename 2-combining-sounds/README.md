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

## Notes

Mixing music is a more subtle problem than I initially anticipated. 
- You can't sum up the waves since they'll go out of range [-1, 1]. 
- You can't just divide amplitude by N or each voice gets too quiet.
- You can't run the sum of amplitudes through something like a ceiling function or tanh() or they sounds gets distorted.

In practice I learned digital music is actually mixed with a *lookahead limiter*. An algorithm that looks for peaks that go beyond [-1, 1], and gently smooth out those peaks.
