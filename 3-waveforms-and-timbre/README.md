# waveforms-and-timbre

Moving beyond the pure sine wave to synthesize richer sounds by controlling their harmonic content — the thing our ears perceive as *timbre*.

Sine waves are a single frequency. Real instruments (and classic synths) are full of harmonics. This experiment generates the classic synth waveforms from scratch and explores how their harmonic structure shapes the sound.

## Build

```
gcc -o main main.c -lm
```

## Run

```
./main
aplay out.wav
```

## Waveforms to build

- **Sawtooth** — contains all harmonics (1/n falloff); bright and buzzy. The starting point for most subtractive synths.
- **Square / pulse** — odd harmonics only; hollow and reedy. Sweeping the pulse *width* (PWM) is a classic effect.
- **Triangle** — odd harmonics with steep falloff; soft and flute-like.
- **Band-limited versions** — naive saws/squares alias badly at high notes. Explore additive band-limiting (summing a finite number of harmonics) to keep them clean.

## Shaping the sound

- **ADSR envelopes** — attack/decay/sustain/release amplitude shaping so notes pluck, swell, or fade instead of clicking on and off.
- **Additive synthesis** — build a tone by summing sine partials with chosen amplitudes; reconstruct a sawtooth from its Fourier series and hear the harmonics add up.
- **Formants** — fixed resonant peaks that make a sound read as a vowel ("ah", "ee", "oo"). Approximate vocal timbres by emphasizing formant frequency bands, laying groundwork for filters in project 4.

## Waveform sketches

```c
float phase = fmodf(freq * t, 1.0f);        // 0..1 ramp per cycle
float saw    = 2.0f * phase - 1.0f;
float square = phase < 0.5f ? 1.0f : -1.0f;
float tri    = 2.0f * fabsf(2.0f * phase - 1.0f) - 1.0f;
```
