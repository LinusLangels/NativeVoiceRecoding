//
//  WriteWaveFile.h
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-10-23.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//

#ifndef ___WriteWaveFile_H__
#define ___WriteWaveFile_H__

#include "stdint.h"

typedef void(*StringParameterCallback)(const char*);

typedef struct {
    char     chunk_id[4];
    uint32_t chunk_size;
    char     format[4];
    char     fmtchunk_id[4];
    uint32_t fmtchunk_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bps;
    char     datachunk_id[4];
    uint32_t datachunk_size;
} WavHeader;

#ifdef __cplusplus
extern "C" {
#endif
	void SetWavDebugLog(StringParameterCallback logger);
	void OpenWavFile(const char* path);
	void SaveSamples(void *buffer, uint32_t sampleSize, uint32_t sampleCount);
	void CloseWavFile();
#ifdef __cplusplus
}
#endif

#endif /* defined(__Plotagon_RecordVoice__WriteWaveFile__) */
