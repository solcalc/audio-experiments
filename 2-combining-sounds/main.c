// Generate a WAV file demonstrating a C major chord: first each note played
// individually, then the full chord run through a series of shaping curves.
// 16-bit PCM, written from scratch.

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include "clamps.h"

#define MAX_INT16         32767
#define PI                3.14159265358879323846
#define SAMPLE_RATE       44100
#define NOTE_DURATION_SEC 1.0
#define SILENCE_SEC       0.1   // gap inserted between segments

// C major chord: C4, E4, G4
const float NOTE_FREQ[] = { 261.63f, 329.63f, 392.00f };
const int NUM_CHORD_NOTES = sizeof(NOTE_FREQ) / sizeof(NOTE_FREQ[0]);

struct __attribute__((packed)) WAVHeader {
    // RIFF chunk (12 bytes)
    char ckID[4];             // "RIFF"
    uint32_t chunkSize;       // 36 + dataSize
    char wvID[4];             // "WAVE"

    // fmt subchunk (24 bytes)
    char ckMarker[4];         // "fmt "
    uint32_t fmtLen;          // 16 for PCM
    uint16_t fmtType;         // 1 = uncompressed PCM
    uint16_t nChannels;
    uint32_t nSampleRate;     // e.g. 44100
    uint32_t nAvgBytesPerSec; // nSampleRate * nChannels * (wBitsPerSample / 8)
    uint16_t nBlockAlign;     // nChannels * (wBitsPerSample / 8)
    uint16_t wBitsPerSample;

    // data subchunk header (8 bytes, followed by samples)
    char dataHeader[4];       // "data"
    uint32_t dataSize;        // nSampleRate * nChannels * (wBitsPerSample / 8) * seconds
};

struct SoundData {
    int16_t* samples;
    uint32_t numSamples;
};

// Audio generation
int16_t generateNoteAmplitude(float freq, float t);
struct SoundData generateNote(float freq, uint32_t numSamples);
struct SoundData generateChord(float* freqs, uint numFreqs, uint32_t numSamples, float* shape_lut);

// WAV file output
void createWAV();
struct WAVHeader createHeader();
void updateHeaderSize(FILE* file, uint32_t dataSize);

// WAV header inspection (debugging helpers)
struct WAVHeader readHeader(const char* fpath);
void printHeader(struct WAVHeader header);

int main() {
    init_lut_functions(); // initialize the lookup tables for shaping functions

    createWAV();

    printf("Playing output...\n");
    system("aplay out.wav -q\n");

    return 0;
}

// Amplitude of a sine wave at frequency `freq` and time `t`, scaled to int16.
int16_t generateNoteAmplitude(float freq, float t) {
    float s = sin(2 * PI * freq * t);
    return (int16_t)(MAX_INT16 * s);
}

// Generate PCM samples for a single note of the given frequency and length.
struct SoundData generateNote(float freq, uint32_t numSamples) {
    int16_t* samples = (int16_t*)malloc(numSamples * sizeof(int16_t));

    for (uint32_t i = 0; i < numSamples; i++) {
        float t = (float)i / SAMPLE_RATE;
        samples[i] = generateNoteAmplitude(freq, t);
    }

    struct SoundData soundData = { samples, numSamples };
    return soundData;
}

// Mix several notes into a chord, then run the result through a shaping curve.
struct SoundData generateChord(float* freqs, uint numFreqs, uint32_t numSamples, float* shape_lut) {
    int16_t* samples = (int16_t*)malloc(numSamples * sizeof(int16_t));
    float* mixed = (float*)malloc(numSamples * sizeof(float));
    struct SoundData notes[numFreqs];

    for (uint32_t note = 0; note < numFreqs; note++)
        notes[note] = generateNote(freqs[note], numSamples);

    // pass 1: sum notes, find the raw peak
    float peak = 0.0f;
    for (uint32_t i = 0; i < numSamples; i++) {
        float mixedSample = 0.0f;
        for (uint32_t note = 0; note < numFreqs; note++)
            mixedSample += (float)notes[note].samples[i];

        mixed[i] = mixedSample;
        float mag = fabsf(mixedSample);
        if (mag > peak) peak = mag;
    }

    // pass 2: normalize, shape, find the shaped peak
    float norm = (peak > 0.0f) ? 1.0f / peak : 0.0f;
    float shapedPeak = 0.0f;
    for (uint32_t i = 0; i < numSamples; i++) {
        float shaped = lut_lookup(shape_lut, mixed[i] * norm);
        mixed[i] = shaped;
        float mag = fabsf(shaped);
        if (mag > shapedPeak) shapedPeak = mag;
    }

    // pass 3: scale up to full scale
    float gain = (shapedPeak > 0.0f) ? (float)MAX_INT16 / shapedPeak : 0.0f;
    for (uint32_t i = 0; i < numSamples; i++)
        samples[i] = (int16_t)(mixed[i] * gain);

    for (uint32_t note = 0; note < numFreqs; note++)
        free(notes[note].samples);
    free(mixed);

    struct SoundData out = { samples, numSamples };
    return out;
}

void createWAV() {
    printf("Generating wav file...\n");

    FILE* out_file = fopen("out.wav", "wb");
    struct WAVHeader header = createHeader();
    fwrite(&header, sizeof(struct WAVHeader), 1, out_file);

    uint32_t dataSize = 0; // running total of audio bytes written
    uint32_t numSamples = (uint32_t)(SAMPLE_RATE * NOTE_DURATION_SEC);

    // short silence written between segments so they're easy to tell apart
    uint32_t silenceSamples = (uint32_t)(SAMPLE_RATE * SILENCE_SEC);
    int16_t* silence = (int16_t*)calloc(silenceSamples, sizeof(int16_t));

    // first, play the notes of the chord one at a time
    for (int note = 0; note < NUM_CHORD_NOTES; note++) {
        struct SoundData soundData = generateNote(NOTE_FREQ[note], numSamples);
        fwrite(soundData.samples, sizeof(int16_t), soundData.numSamples, out_file);
        dataSize += soundData.numSamples * sizeof(int16_t);
        free(soundData.samples);

        fwrite(silence, sizeof(int16_t), silenceSamples, out_file);
        dataSize += silenceSamples * sizeof(int16_t);
    }

    // then play the whole chord through each shaping curve
    float* shapers[] = { lut_hardclip, lut_tanh, lut_rational1, lut_rational2, lut_rational5, lut_rational10 };
    int numShapers = sizeof(shapers) / sizeof(shapers[0]);

    for (int s = 0; s < numShapers; s++) {
        struct SoundData chordData = generateChord((float*)NOTE_FREQ, NUM_CHORD_NOTES, numSamples, shapers[s]);
        fwrite(chordData.samples, sizeof(int16_t), chordData.numSamples, out_file);
        dataSize += chordData.numSamples * sizeof(int16_t);
        free(chordData.samples);

        fwrite(silence, sizeof(int16_t), silenceSamples, out_file);
        dataSize += silenceSamples * sizeof(int16_t);
    }

    updateHeaderSize(out_file, dataSize);
    free(silence);
    fclose(out_file);
    printf("Wav generation complete!\n");
}

struct WAVHeader createHeader() {
    struct WAVHeader out;

    // fixed character tags
    memcpy(out.ckID, "RIFF", 4);
    memcpy(out.wvID, "WAVE", 4);
    memcpy(out.ckMarker, "fmt ", 4);
    memcpy(out.dataHeader, "data", 4);

    // format properties
    out.fmtLen = 16;
    out.fmtType = 1;
    out.nChannels = 1;
    out.nSampleRate = SAMPLE_RATE;
    out.wBitsPerSample = 16;

    // derived properties
    out.nAvgBytesPerSec = out.nSampleRate * out.nChannels * (out.wBitsPerSample / 8);
    out.nBlockAlign = out.nChannels * (out.wBitsPerSample / 8);

    // placeholder sizes; patched by updateHeaderSize after audio is written
    out.dataSize = 0;
    out.chunkSize = out.dataSize + sizeof(struct WAVHeader) - 8;

    return out;
}

void updateHeaderSize(FILE* file, uint32_t dataSize) {
    uint32_t chunkSize = dataSize + sizeof(struct WAVHeader) - 8;

    fseek(file, offsetof(struct WAVHeader, chunkSize), SEEK_SET);
    fwrite(&chunkSize, sizeof(uint32_t), 1, file);

    fseek(file, offsetof(struct WAVHeader, dataSize), SEEK_SET);
    fwrite(&dataSize, sizeof(uint32_t), 1, file);

    fseek(file, 0, SEEK_END); // restore position in case more is written
}

struct WAVHeader readHeader(const char* fpath) {
    FILE* file = fopen(fpath, "rb");
    printf("Reading WAV header from file: %s\n", fpath);
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    struct WAVHeader header;
    if (fread(&header, sizeof(struct WAVHeader), 1, file)) {
        fclose(file);
        printf("Read %zu bytes from file\n", sizeof(struct WAVHeader));
        return header;
    } else {
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }
}

void printHeader(struct WAVHeader header) {
    printf("ckID: %.*s\n", 4, header.ckID);
    printf("chunkSize: %u\n", header.chunkSize);
    printf("wvID: %.*s\n", 4, header.wvID);
    printf("ckMarker: %.*s\n", 4, header.ckMarker);
    printf("fmtLen: %u\n", header.fmtLen);
    printf("fmtType: %u\n", header.fmtType);
    printf("nChannels: %u\n", header.nChannels);
    printf("nSampleRate: %u\n", header.nSampleRate);
    printf("nAvgBytesPerSec: %u\n", header.nAvgBytesPerSec);
    printf("nBlockAlign: %u\n", header.nBlockAlign);
    printf("wBitsPerSample: %u\n", header.wBitsPerSample);
    printf("dataHeader: %.*s\n", 4, header.dataHeader);
    printf("dataSize: %u\n", header.dataSize);

    double duration = (double)header.dataSize / header.nAvgBytesPerSec;
    printf("Duration: %.2f seconds\n", duration);
}
