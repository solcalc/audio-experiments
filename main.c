// Generate a WAV file with a C major scale (C4 to C5) using 16-bit PCM encoding from scratch.

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_INT16 32767
#define PI 3.14159265358879323846
#define NUM_NOTES 8
#define NOTE_DURATION_SEC 1
#define SAMPLE_RATE 44100

// WAV file layout:
//   [RIFF chunk: 12 bytes]
//     "RIFF" (4) | chunkSize (4) | "WAVE" (4)
//   [fmt subchunk: 24 bytes]
//     "fmt " (4) | fmtLen (4) | fmtType (2) | nChannels (2) |
//     nSampleRate (4) | nAvgBytesPerSec (4) | nBlockAlign (2) | wBitsPerSample (2)
//   [data subchunk: 8 + dataSize bytes]
//     "data" (4) | dataSize (4) | <samples>
struct __attribute__((packed)) WAVHeader{
    // RIFF chunk (12 bytes)
    char ckID[4]; // "RIFF"
    uint32_t chunkSize; // 36 + dataSize
    char wvID[4]; // "WAVE"

    // fmt subchunk (24 bytes)
    char ckMarker[4]; // "fmt "
    uint32_t fmtLen; // 16 for PCM
    uint16_t fmtType; // 1 = uncompressed PCM
    uint16_t nChannels;
    uint32_t nSampleRate; // e.g. 44100
    uint32_t nAvgBytesPerSec; // nSampleRate * nChannels * (wBitsPerSample / 8)
    uint16_t nBlockAlign; // nChannels * (wBitsPerSample / 8)
    uint16_t wBitsPerSample;

    // data subchunk header (8 bytes, followed by samples)
    char dataHeader[4]; // "data"
    uint32_t dataSize; // nSampleRate * nChannels * (wBitsPerSample / 8) * seconds
};

struct WAVHeader readHeader(const char* fpath);
void printHeader(struct WAVHeader header);
struct WAVHeader createHeader();
void createWAV();
int16_t generateNoteAmplitude(float freq, float t);

// C major scale frequencies (C4 to C5)
const float NOTE_FREQ[] = { 261.63f, 293.66f, 329.63f, 349.23f, 392.00f, 440.00f, 493.88f, 523.25f };

int main() {

    const char* fpath = "out.wav";
    // struct WAVHeader header = readHeader(fpath);
    // printHeader(header);
    createWAV();
    


    return 0;
}

void createWAV() {
    printf("Generating wav file...");

    FILE* out_file = fopen("out.wav", "wb");
    struct WAVHeader header = createHeader();
    fwrite(&header, sizeof(struct WAVHeader), 1, out_file);

    for (int note = 0; note < 8; note++) {
        int32_t t = 0;
        while (t < SAMPLE_RATE * NOTE_DURATION_SEC) {
            int16_t sample = generateNoteAmplitude(NOTE_FREQ[note], (float) t  / (float) SAMPLE_RATE);
            fwrite(&sample, sizeof(int16_t), 1, out_file);
            t++;
        }
    }

    fclose(out_file);
    printf("Wav generation complete!");
}

int16_t generateNoteAmplitude(float freq, float t) {
    float s = sin(2 * PI * freq * t);
    int16_t out = (int16_t) (MAX_INT16 * s);
    return out;
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
    }

    else {
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }
}

struct WAVHeader createHeader() {
    struct WAVHeader out;

    // fixed characters headers
    memcpy(out.ckID, "RIFF", 4);
    memcpy(out.wvID, "WAVE", 4);
    memcpy(out.ckMarker, "fmt ", 4);
    memcpy(out.dataHeader, "data", 4);

    // numerical properties
    out.fmtLen = 16;
    out.fmtType = 1;
    out.nChannels = 1;
    out.nSampleRate = SAMPLE_RATE;
    out.wBitsPerSample = 16;

    // derived members
    out.nAvgBytesPerSec = out.nSampleRate * out.nChannels * (out.wBitsPerSample / 8);
    out.nBlockAlign = out.nChannels * (out.wBitsPerSample / 8);
    out.dataSize = (uint32_t)(out.nSampleRate * out.nChannels * (out.wBitsPerSample / 8) * NUM_NOTES * NOTE_DURATION_SEC);
    out.chunkSize = out.dataSize + 44 - 8;

    return out;
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
