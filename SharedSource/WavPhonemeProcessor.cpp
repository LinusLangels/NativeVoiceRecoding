//
//  WavPhonemeProcessor.cpp
//  MicrophoneRecorder
//
//  Created by John Philipsson on 2016-07-13.
//  Copyright © 2016 John Philipsson. All rights reserved.
//

#define _CRT_SECURE_NO_WARNINGS //Allow strcpy on windows.
#define _CRT_SECURE_NO_DEPRECATE //Allow fopen on windows.

#include "WavPhonemeProcessor.h"
#include "WriteWaveFile.h"
#include "VoiceRecordingUtils.h"
#include "stdio.h"
#include <stdlib.h>
#include <cstring>
#include <string>

voiceRec_t *voicePhonemes;

WavPhonemeProcessor::WavPhonemeProcessor(const char* path)
{
    m_wavFilePath = path;
}

void WavPhonemeProcessor::setPhonemeLogger(PhonemeParameterCallback phonemeLogger)
{
    m_PhonemeLogger = phonemeLogger;
}

void WavPhonemeProcessor::CalulcatePhonemesFromWav()
{
    //Open wav file for reading.
    FILE *wavFileHandle = fopen(m_wavFilePath, "rb");
    
    //Read Wav header.
    WavHeader *header = (WavHeader*)malloc(sizeof(WavHeader));
    fread(header, sizeof(WavHeader), 1, wavFileHandle);
    
    uint32_t audioDataLength = header->datachunk_size;
    uint32_t sampleRate = header->sample_rate;
    uint16_t byteRate = header->bps / 8;
   
    int samplesinfile = audioDataLength/byteRate;
    
    //Setup.
    int bufferSize = 1024;
    
    //Allocate memory for readbuffer and floating-point audiobuffer
    void * readFrame = (void*) malloc(bufferSize*byteRate);
  
    //Sanity check, make sure readbuffer was allocated.
    if (readFrame == NULL)
    {
        fclose(wavFileHandle);
        free(header);
        return;
    }
    
    float * floatFrame = (float*) malloc(bufferSize*sizeof(float));
    
    //Sanity check, make sure floating-point audiobuffer was allocated.
    if (floatFrame == NULL)
    {
        free(readFrame);
        fclose(wavFileHandle);
        free(header);
        return;
    }
    
    //set-up phoneme algos init
    voicePhonemes = CalculatePhoenems_init(sampleRate,512,true,0,true);
    
    int gSampleCounter = 0;
    int samplesread = 0;
    while (samplesinfile >= bufferSize)
    {
        samplesread = (int)fread(readFrame, sizeof(byteRate), bufferSize, wavFileHandle);
        samplesinfile = samplesinfile - samplesread;
        
        if (0 == samplesread && feof(wavFileHandle))
            break;
        
        for(int i = 0; i < samplesread;i++)
        {
            floatFrame[i] = ((float)((short*)readFrame)[i])/32767;
        }
        
        //Call Phonemes Extraction
        if(samplesread > 512)
        {
            
            float QuephonemeStarttime = (((float)gSampleCounter) / (voicePhonemes->fs));
            gSampleCounter = gSampleCounter + samplesread;
            float QuephonemeStoptime = (((float)gSampleCounter) / (voicePhonemes->fs));
            
            //buffer is 512 samples
            voicePhonemes = CalculatePhoenems(floatFrame,voicePhonemes->signalE,voicePhonemes->vow,voicePhonemes,voicePhonemes->preVoice,voicePhonemes->dec,voicePhonemes->preDist,voicePhonemes->evad);
            
            if (m_PhonemeLogger != NULL)
            {
                struct PhonemeData phoneme;
                phoneme.start = QuephonemeStarttime;
                phoneme.stop = QuephonemeStoptime;
                phoneme.phoneme = voicePhonemes->phoneme;
                
                m_PhonemeLogger(&phoneme);
            } 
        }
    }
    
     //tear-down phoneme algos 
    CalculatePhoenems_destroy(voicePhonemes,0);
    
    //close the wav-file.
    fclose(wavFileHandle);
    
    //Free all dynamic memory.
    free(readFrame);
    free(floatFrame);
    free(header);
}

WavPhonemeProcessor::~WavPhonemeProcessor()
{
    if (m_PhonemeLogger != nullptr)
    {
        //m_PhonemeLogger("Destroyed wav phoneme processor!");
    }
    
    m_PhonemeLogger = nullptr;
}

