# learning-wav

A learning project for reading and writing WAV files in C from scratch, without any audio libraries.

Generates a C major scale (C4 to C5) as a 16-bit mono PCM WAV file by computing sine waves for each note frequency and writing raw samples directly to disk.

## Build

```
gcc -o main main.c -lm
```

## Run

```
./main
aplay out.wav
```

## WAV file structure

A WAV file is a RIFF container with three chunks:

```
[RIFF chunk: 12 bytes]
  "RIFF" (4) | chunkSize (4) | "WAVE" (4)
[fmt subchunk: 24 bytes]
  "fmt " (4) | fmtLen (4) | fmtType (2) | nChannels (2) |
  nSampleRate (4) | nAvgBytesPerSec (4) | nBlockAlign (2) | wBitsPerSample (2)
[data subchunk: 8 + dataSize bytes]
  "data" (4) | dataSize (4) | <samples>
```

Samples are signed 16-bit integers (`int16_t`), generated as:

```c
sample = (int16_t)(sin(2 * PI * freq * t) * 32767);
```
