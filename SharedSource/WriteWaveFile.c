//#define VERBOSE_LOGGING
//
//  WriteWaveFile.c
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-10-23.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//

#define _CRT_SECURE_NO_DEPRECATE //Allow fopen on windows.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "WriteWaveFile.h"

#define BYTE_RATE 2
#define SAMPLE_RATE 44100
#define NUM_CHANNELS 1
#define AUDIOFORMAT_PCM 1

FILE *m_WavFileHandle;
WavHeader *wavHeader;
uint32_t bytesWritten;

StringParameterCallback wavDebugLog;

void SetWavDebugLog(StringParameterCallback logger)
{
    wavDebugLog = logger;
}

void OpenWavFile(const char* path)
{
    //Reset byte counter.
    bytesWritten = 0;
    
    wavHeader = (WavHeader*)malloc(sizeof(WavHeader));
    //strcpy seems to crash on some platforms. Use strncpy instead.
    strncpy(wavHeader->chunk_id, "RIFF ", 4);
    wavHeader->chunk_size = (uint32_t)0;
    strncpy(wavHeader->format, "WAVE", 4);
    strncpy(wavHeader->fmtchunk_id, "fmt ", 4);
    wavHeader->fmtchunk_size = (uint32_t)16; //Constant
    wavHeader->audio_format = (uint16_t)AUDIOFORMAT_PCM; //PCM format is 1.
    wavHeader->num_channels = (uint16_t)NUM_CHANNELS; //Mono
    wavHeader->sample_rate = (uint32_t)SAMPLE_RATE;
    wavHeader->byte_rate = (uint32_t)(SAMPLE_RATE * NUM_CHANNELS * BYTE_RATE);
    wavHeader->block_align = (uint16_t)(NUM_CHANNELS * BYTE_RATE); // nChannels * (bitsPerSample / 8)
    wavHeader->bps= (uint16_t)(BYTE_RATE * 8); //8 * bytes per sample.
    strncpy(wavHeader->datachunk_id, "data", 4);
    wavHeader->datachunk_size= (uint32_t)0;
    
    m_WavFileHandle = fopen(path,"wb");
    
    if (m_WavFileHandle != NULL)
    {
		#ifdef VERBOSE_LOGGING
        if (wavDebugLog != NULL)
        {
            wavDebugLog("writing wav header.");
        }
		#endif
        
        //Write initial header values to file. Later update with size info.
        fwrite(wavHeader, sizeof(WavHeader), 1, m_WavFileHandle);
    }
    else
    {
        if (wavDebugLog != NULL)
        {
            wavDebugLog("error opening wav file.");
        }
    }
}

void SaveSamples(void *buffer, uint32_t sampleSize, uint32_t sampleCount)
{
    //Track bytes written. Used to derive wav file block length.
    bytesWritten += sampleCount * sampleSize;
    
    if (m_WavFileHandle != NULL)
    {
		#ifdef VERBOSE_LOGGING
        if (wavDebugLog != NULL)
        {
            wavDebugLog("writing samples");
        }
		#endif
        
        fwrite(buffer, sampleSize, sampleCount, m_WavFileHandle);
    }
}

void CloseWavFile()
{
    if (m_WavFileHandle != NULL)
    {
        //Rewind file so we can resave the header data.
        fseek(m_WavFileHandle, 0, SEEK_SET);
    
        if (wavHeader != NULL)
        {
            //Adjust length values and resave header data.
            wavHeader->chunk_size = (uint32_t)(bytesWritten + 36); // Exclude RIFF AND WAVE bytes.
            wavHeader->datachunk_size = (uint32_t)bytesWritten; //Length of all samples in bytes.

            //Write updated header to file.
            fwrite(wavHeader, sizeof(WavHeader), 1, m_WavFileHandle);
		}
    
        //We are done, close the file.
        fclose(m_WavFileHandle);
        
		#ifdef VERBOSE_LOGGING
        if (wavDebugLog != NULL)
        {
            wavDebugLog("closing wav file");
        }
		#endif
    }
    else
    {
		#ifdef VERBOSE_LOGGING
		if (wavDebugLog != NULL)
		{
		wavDebugLog("error closing wav file");
		}
		#endif
    }
    
    if (wavHeader != NULL)
    {
        //Release memory for header.
        free(wavHeader);
    }
    
    //Invalidate pointers
    m_WavFileHandle = NULL;
    wavHeader = NULL;
}

