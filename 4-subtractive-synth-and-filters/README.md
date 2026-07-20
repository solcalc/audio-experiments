# subtractive-synth-and-filters

Digital signal processing: taking the harmonically-rich waveforms from project 3 and *carving* them with filters — the heart of subtractive synthesis — plus a first look at effects and analysis.

Where project 3 was about generating harmonics, this one is about selectively removing and reshaping them. This is where DSP fundamentals come in.

## Build

```
gcc -o main main.c -lm
```

## Run

```
./main
aplay out.wav
```

## Directions to explore

### Filters
- **One-pole low-pass / high-pass** — the simplest IIR filters; feed a sawtooth in and hear it soften as the cutoff drops.
- **Biquad filters** — resonant low-pass with cutoff *and* resonance (Q). Sweeping the cutoff of a resonant filter over a saw is *the* classic synth sound.
- **Filter envelopes** — modulate cutoff with an ADSR so each note has a "wah" sweep.

### Effects (delay-line based)
- **Delay / echo** — a circular buffer of past samples fed back with decay.
- **Reverb** — combs + all-pass filters (Schroeder reverb) to fake a room.
- **Chorus / flanger** — a modulated short delay for thickening.

### Analysis
- **DFT / FFT** — transform a signal into its frequency spectrum to *see* the harmonics from project 3 and verify what the filters are doing.

## Possible alternate direction: MIDI

Instead of hardcoding notes, parse a `.mid` file and drive the synth from it — note-on/note-off events, velocities, timing. Turns the whole stack into a playable instrument.

## Filter sketch (one-pole low-pass)

```c
// a in (0,1): smaller = lower cutoff
float y = 0.0f;
for (each sample x) {
    y = y + a * (x - y);   // leaky integrator
    output = y;
}
```
