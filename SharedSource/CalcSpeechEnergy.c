//
//  CalcSpeechEnergy.c
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-10-05.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//

#include "CalcSpeechEnergy.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>



signalEnergy_t* calcSpeechEnergy_init(int shortFrameSize, int longFrameSize)
{
    int i;
    
    signalEnergy_t *signalE = (signalEnergy_t *)malloc(sizeof(signalEnergy_t));
    
    if(signalE == NULL)
    {
        return NULL;
    }
    
    signalE->shortFrameSize = shortFrameSize;
    signalE->longFrameSize = longFrameSize;
    
    signalE->shortEnergy = 0;
    signalE->longEnergy = 0;
    signalE->speechStateChange = 0;
    signalE->previousSpeechState = 0;
    signalE->speechdetected = false;
    signalE->maxLongPower = -1;
    signalE->maxShortPower = -1;
    
    signalE->sizeOfLongEnergymemory = longFrameSize/shortFrameSize;
    
    for(i=0;i<signalE->sizeOfLongEnergymemory;i++)
    {
        signalE->longEnergyMemory[i] = 0;
    }
    
    
    signalE->speechdetectionmemory = 0;
    signalE->speechdetectionmemorycount = 0;
    signalE->hangover_target = 5;
    signalE->hangover_count = 0;
    signalE->countdownstarted = 0;
    signalE->silence_target = 10;
    signalE->static_update_target = 10;
    signalE->BackgroundCurrentE = 0;
    signalE->BackgroundMeanE = 0.015;
    signalE->BackGroundFilterCoeff = 0.15;
    signalE->BackgroundMeanE_prev = signalE->BackgroundMeanE;
    signalE->BackgroundStatic = signalE->BackgroundMeanE;
    signalE->update_count = 0;
    signalE->silencecounter = 0;
    
    signalE->lowestPowerLevel = 0.005; 
    
    return signalE;
    
}

void calcSpeechEnergy_destroy(signalEnergy_t *signalE)
{
    free(signalE);
}

signalEnergy_t* calcSpeechEnergy_exec(signalEnergy_t * signalE, float *framebuffer)
{
    int i;
    
    
    //calculate Energy (Long and Short)
    
    signalE->shortEnergy = 0;
    for(i=0;i<signalE->shortFrameSize;i++)
    {
        signalE->shortEnergy = signalE->shortEnergy + fabs(framebuffer[i]);
    }
    signalE->shortEnergy = signalE->shortEnergy / signalE->shortFrameSize;
    
    
    if(signalE->shortEnergy > signalE->maxShortPower)
        signalE->maxShortPower = signalE->shortEnergy;
    
    
    //update longEnergyMemory
    for (i = signalE->sizeOfLongEnergymemory-1; i > 0 ; i--)
    {
        signalE->longEnergyMemory[i] = signalE->longEnergyMemory[i-1];
    }
    signalE->longEnergyMemory[0] = signalE->shortEnergy;
    
    
    
    signalE->longEnergy = 0;
    for(i=0;i<signalE->sizeOfLongEnergymemory;i++)
    {
        signalE->longEnergy = signalE->longEnergy + signalE->longEnergyMemory[i];
    }
    signalE->longEnergy = signalE->longEnergy / signalE->sizeOfLongEnergymemory;
    
    
    if(signalE->longEnergy > signalE->maxLongPower)
        signalE->maxLongPower = signalE->longEnergy;
    
    
    //calculate Energy differences
    signalE->speechStateChange = 0;
    
    if((signalE->shortEnergy > 1.2*signalE->longEnergy) && (signalE->shortEnergy > 1.5*signalE->BackgroundStatic) && (signalE->longEnergy > 1.5*signalE->BackgroundStatic)) // 1.2 , 1.3 , 1.3
    {
        
        if((signalE->longEnergy) > (signalE->lowestPowerLevel))
        {
            signalE->speechdetectionmemorycount++;
            
            if(signalE->speechdetectionmemorycount > signalE->speechdetectionmemory)
            {
                signalE->speechStateChange = 1;
                signalE->previousSpeechState = 1;
                signalE->hangover_count = signalE->hangover_target;
                signalE->countdownstarted = 0;
                signalE->silencecounter = 0;
                signalE->speechdetected = true;
            }
        }
      
    }
    else
    {
        signalE->speechdetectionmemorycount=0;
    }
    
    
    if(signalE->countdownstarted == 1)
    {
        signalE->hangover_count--;
        if(signalE->hangover_count < 0)
        {
            signalE->speechStateChange = 1;
            signalE->previousSpeechState = -1;
            signalE->hangover_count = 0;
            signalE->countdownstarted = 0;
            signalE->speechdetectionmemorycount = 0;
            signalE->speechdetected = false;
        }
    }
    
    
    if((signalE->shortEnergy < 0.7*signalE->longEnergy) && (signalE->shortEnergy < 1.5*signalE->BackgroundStatic)) // 0.7 , 1.3
    {
        if(signalE->hangover_count == 0)
        {
            signalE->hangover_count = signalE->hangover_target;
        }
        signalE->countdownstarted = 1;
     
    }
    
    
    if(signalE->speechStateChange == 0)
    {
        if(signalE->previousSpeechState == 1)
            signalE->speechdetected = true;
        else
            signalE->speechdetected = false;
        
    }
    
    if(signalE->countdownstarted == 0)
    {
        if((signalE->shortEnergy < 1.5*signalE->BackgroundStatic) && (signalE->longEnergy < 1.5*signalE->BackgroundStatic)) //1.3 , 1.3
        {
            signalE->speechdetected = false;
            signalE->previousSpeechState = -1;
        }
        
    }
    
    if(signalE->previousSpeechState == -1)
    {
        signalE->silencecounter++;
    }
    

    if((signalE->previousSpeechState == -1) && (signalE->countdownstarted == 0) && (signalE->silencecounter > signalE->silence_target))
    {
        signalE->BackgroundMeanE_prev = signalE->BackgroundMeanE;
        signalE->BackgroundCurrentE = signalE->shortEnergy;
        signalE->BackgroundMeanE = (1-signalE->BackGroundFilterCoeff)*signalE->BackgroundMeanE + signalE->BackGroundFilterCoeff*signalE->BackgroundCurrentE;
        
        signalE->update_count++;
        
        if(signalE->update_count == signalE->static_update_target)
        {
            signalE->BackgroundStatic = signalE->BackgroundMeanE;
            signalE->update_count = 0;
            if(signalE->BackgroundStatic > 0.005)
            {
                signalE->hangover_target = 10;
            }
            if(signalE->BackgroundStatic > 0.01)
            {
                signalE->hangover_target = 15;
            }
            if(signalE->BackgroundStatic > 0.015)
            {
                signalE->hangover_target = 20;
            }
            if(signalE->BackgroundStatic < 0.0005)
            {
                signalE->hangover_target = 5;
            }
        }
    }
    else
    {
        signalE->BackgroundMeanE = signalE->BackgroundStatic;
        signalE->update_count = 0;
        
    }
    
    return signalE;
    
}



