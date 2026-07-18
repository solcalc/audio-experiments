#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define SAMPLE_RATE 44100
#define NUM_CHANNELS 1
#define BITS_PER_SAMPLE 16
#define NOTE_DURATION_SEC 1.0

struct __attribute__((packed)) WAVHeader{
    // RIFF Chunk descriptor
    char ckID[4]; // "RIFF"
    uint32_t chunkSize; // File size - 8
    char wvID[4]; // "WAVE"

    // "fmt " sub chunk
    char ckMarker[4]; // "fmt "
    uint32_t fmtLen;
    uint16_t fmtType;
    uint16_t nChannels;
    uint32_t nSampleRate; // e.g. 44100
    uint32_t nAvgBytesPerSec; // Data rate
    uint16_t nBlockAlign; // Data block size (bytes)
    uint16_t wBitsPerSample;
    char dataHeader[4]; // "data"
    uint32_t dataSize;
};

struct WAVHeader readHeader(const char* fpath) {
    FILE* file = fopen(fpath, "rb");
    printf("Reading WAV header from file: %s\n", fpath);
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    struct WAVHeader header;
    if (fread(&header, sizeof(header), 1, file)) {
        fclose(file);
        printf("Read %zu bytes from file\n", sizeof(header));
        return header;
    }

    else {
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }
}

int main() {

    const char* fpath = "sample.wav";
    struct WAVHeader header = readHeader(fpath);

    return 0;
    

}
