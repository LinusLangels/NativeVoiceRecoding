//
//  CalcSpeechEnergy.h
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-10-05.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//

#ifndef __Plotagon_RecordVoice__CalcSpeechEnergy__
#define __Plotagon_RecordVoice__CalcSpeechEnergy__

#include <stdio.h>
#include <stdbool.h>


typedef struct signalEnergy_st {
    
    int longFrameSize;
    int shortFrameSize;
    float shortEnergy;
    float longEnergy;
    int speechStateChange;
    int previousSpeechState;
    bool speechdetected;
    float maxShortPower;
    float maxLongPower;
    
    float longEnergyMemory[4];
    int sizeOfLongEnergymemory;
    
    int speechdetectionmemory;
    int speechdetectionmemorycount;
    int hangover_target;
    int hangover_count;
    int countdownstarted;
    
    int silence_target;
    int static_update_target;
    
    float BackgroundMeanE;
    float BackGroundFilterCoeff;
    float BackgroundCurrentE;
    float BackgroundMeanE_prev;
    float BackgroundStatic;
    
    float lowestPowerLevel;
    
    int update_count;
    int silencecounter;
    
}signalEnergy_t;

signalEnergy_t* calcSpeechEnergy_init(int shortFrameSize, int longFrameSize);
void calcSpeechEnergy_destroy(signalEnergy_t * signalE);
signalEnergy_t* calcSpeechEnergy_exec(signalEnergy_t * signalE, float *framebuffer);

#endif /* defined(__Plotagon_RecordVoice__CalcSpeechEnergy__) */
