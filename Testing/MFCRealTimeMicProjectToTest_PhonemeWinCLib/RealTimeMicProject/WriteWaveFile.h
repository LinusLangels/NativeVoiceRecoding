//
//  WriteWaveFile.h
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-10-23.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//

#ifndef __Plotagon_RecordVoice__WriteWaveFile__
#define __Plotagon_RecordVoice__WriteWaveFile__

#include <stdio.h>
#include <inttypes.h>

#define WAV_HEADER_BYTES 44


typedef struct WAV_HEADER_DATA
{
    uint8_t     RIFF[4];        // RIFF Header      / //Magic header
    uint32_t    ChunkSize;      // RIFF Chunk Size  /
    uint8_t     WAVE[4];        // WAVE Header      /
    uint8_t     fmt[4];         // FMT header       /
    uint32_t    Subchunk1Size;  // Size of the fmt chunk                                /
    uint16_t    AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM /
    uint16_t    NumOfChan;      // Number of channels 1=Mono 2=Sterio                   /
    uint32_t    SamplesPerSec;  // Sampling Frequency in Hz                             /
    uint32_t    bytesPerSec;    // bytes per second /
    uint16_t    blockAlign;     // 2=16-bit mono, 4=16-bit stereo /
    uint16_t    bitsPerSample;  // Number of bits per sample      /
    uint8_t     fact[4];
    uint32_t    FactChunkSize;
    uint8_t *   FactData;
    uint8_t     Subchunk2ID[4]; // "data"  string   /
    uint32_t    Subchunk2Size;  // Sampled data length    /
    uint16_t    TotalHeaderSize;
}WAV_HEADER_DATA;

typedef struct WAV_FORMAT_DATA
{
    uint16_t Channels;
    uint32_t SampleRate;
    uint32_t BitsPerSample;
}WAV_FORMAT_DATA;




void CreateHeaderToWave(uint8_t * Header, uint32_t BytesRecorded, WAV_FORMAT_DATA PCMfmt);

#endif /* defined(__Plotagon_RecordVoice__WriteWaveFile__) */
