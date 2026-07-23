# chord-gui

A raylib GUI front-end for project 2's C-major chord synthesis. Where project 2
baked everything into a single `out.wav` and played it with `aplay`, this
experiment makes it interactive: generate each part of the chord on demand, play
it back in real time, and *see* the sample data as a waveform.

This is also the first experiment to lean on a library (raylib) instead of
building everything from scratch — raylib handles the window, real-time audio
output, and drawing, while the synthesis itself stays hand-written (reused from
project 2).

## Goals

- **Play each note individually** — C4, E4, G4 as separate buttons.
- **Play the full chord** — the three notes mixed together.
- **Shaping curve selector** — switch between the clipping/saturation curves from
  project 2 (hardclip, tanh, rational 1/2/5/10) and hear how each reshapes the
  chord.
- **Real-time playback** — via raylib's `AudioStream` callback, filling buffers
  with the synthesized samples instead of writing a WAV first.
- **Waveform visualization** — draw the currently generated buffer, with a
  playhead tracking where audio playback is.

## Planned structure

- `synth.{c,h}` — note + chord generation from project 2, ported to `float`
  buffers in `[-1, 1]` so samples feed straight into the audio stream and the
  waveform view.
- `clamps.{c,h}` — the shaping-curve lookup tables (reused from project 2).
- `main.c` — raylib window, audio stream + callback, UI, and waveform drawing.

## Build (planned)

Requires raylib (`pacman -S raylib`):

```
gcc main.c synth.c clamps.c -o main $(pkg-config --cflags --libs raylib) -lm
```

## Run (planned)

```
./main
```

## Status

Scaffold only — README and project direction. Implementation to follow.

