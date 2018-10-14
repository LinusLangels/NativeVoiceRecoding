//
//  EnhancedTalkVsNoiseDetection.c
//  MicrophoneRecorder
//
//  Created by John Philipsson on 2016-01-12.
//  Copyright © 2016 John Philipsson. All rights reserved.
//


/*
 
 This is a robust version of a VAD (Voice Activity Detector)
 
 It will always be a trade-off between avoid false detection of noise/spikes/disturbance interpreted as Voice/Talk,
 to that it actually miss true Voice/Talk as it is interpreted as noise/spikes/disturbance
 
 However, the performance is greater of detecting true Voice from Noise, than the other way around.
 
 One important setting is the robustnessFactor, the higher the more robust (i.e. the better it avoid false detection of Voice/Talk, but can miss some Voice -> Trade-off)
 Also, the robustnessFactor is directly proportional to complexity, so tuning is needed
 Defalut value for robustnessFactor is 150. settings could typically be between (10-300).
 
 An other important setting is the weightingTarget, the higher the more robust (i.e. the better it avoid false detection of Voice/Talk, but can miss some Voice -> Trade-off)
 This does not affect complexity.
 Defalut value for weightingTarget is 0.9. settings could typically be between (0.8-0.95).
 
 This algorithm is ON as default, but can be turned OFF using the lowComplexityMode (fullSearchModeOn) from the main Phoneme init function (CalculatePhoenems_init)
 
 
 */

#include "EnhancedTalkVsNoiseDetection.h"



eVAD_t* ehancedVAD_init(int fs, int framesize, int robustnessFactor, float weightingTarget)
{
    
    int i;
    
    eVAD_t *evad = (eVAD_t *)malloc(sizeof(eVAD_t));
    
    if(evad == NULL)
    {
        return NULL;
    }
    
    evad->robustnessFactor = robustnessFactor;
    evad->weightingTarget = weightingTarget;
    
    evad->eVAD_buffer = (float *)malloc((framesize)* sizeof(float));
    for(i=0;i<framesize;i++)
    {
        evad->eVAD_buffer[i] = 0;
    }
    
    // not in use at the moment
    /*
     evad->MA_buffer = (float *)malloc((robustnessFactor)* sizeof(float));
     for(i=0;i<framesize;i++)
     {
     evad->MA_buffer[i] = 0;
     }
     */
    
    evad->timeExp = (float *)malloc((framesize)* sizeof(float));
    evad->valueExp = (float *)malloc((framesize)* sizeof(float));
    evad->expData = (float *)malloc((framesize)* sizeof(float));
    
    for(i=0;i<framesize;i++)
    {
        evad->timeExp[i] = 0;
        evad->valueExp[i] = 0;
        evad->expData[i]=0;
    }
    
    return evad;
    
}




void ehancedVAD_destroy(eVAD_t *evad)
{
    
    free(evad->eVAD_buffer);
    //free(evad->MA_buffer);  // not in use at the moment
    free(evad->timeExp);
    free(evad->valueExp);
    free(evad->expData);
    
    free(evad);
}





void ehancedVAD_exec(eVAD_t *evad, float *buffer, int lengthOfBuffer, int fs, int robustnessFactor)
{
    
    float exponentValue;
    
    // Make a copy of audio input samples
    for(int i=0;i<lengthOfBuffer;i++)
    {
        evad->eVAD_buffer[i] = buffer[i];
    }
    
    // Calculate a spectral model of self-similar process, and the PSD and Estimate the spectral exponent
    // Adjust bias of spectral exponent and calculate the normalized squared value
    exponentValue = EstimateSpectralExponent(evad,buffer,lengthOfBuffer);
    
    
    
    // run  calculation through a MA-process (not active yet) with target levels
    // produce output, as a weighting vector to manipulate buffer samples
    WeightBufferSamples(evad,lengthOfBuffer,exponentValue);
    
    
    
}


void WeightBufferSamples(eVAD_t *evad, int framelength, float exponentValue)
{
    
    int i;
    
    if(exponentValue > evad->weightingTarget)
    {
        exponentValue=1;
    }
    else
    {
        exponentValue=0;
    }
    
    evad->exponentV = exponentValue;
    
    //chop the data
    for(i=0;i<framelength;i++)
    {
        evad->eVAD_buffer[i] = evad->eVAD_buffer[i]*exponentValue;
    }
    
}




float EstimateSpectralExponent(eVAD_t *evad, float * frame, int framelength)
{
    
    int i;
    
    float * logTimeExp;
    float * logValueExp;
    
    
    float y;
    int m;
    int loglogsize = 0;
    int divBlock = 1;
    
    float biasExponent;
    
    int blockSize,blockSizeTarget;
    
    blockSizeTarget = 4;
    
    for(i=0;i<framelength;i++)
    {
        evad->timeExp[i] = 0;
        evad->valueExp[i] = 0;
        evad->expData[i]=frame[i];
    }
    
    blockSize = framelength;
    
    while( blockSize > blockSizeTarget)
    {
        m = 0;
        float tmp = 0;
        for(int i = 0; i < blockSize; i++)
        {
            m = m + evad->expData[i];
        }
        m = m / blockSize;
        for(int i = 0; i < blockSize; i++)
        {
            tmp = tmp + ((evad->expData[i]-m)*(evad->expData[i]-m));
        }
        
        tmp = tmp/((float)blockSize-1);
        y = sqrtf(tmp);
        evad->timeExp[loglogsize] = divBlock;
        evad->valueExp[loglogsize] = divBlock * y;
        loglogsize++;
        divBlock = divBlock * 2;
        blockSize = blockSize / 2 ;
        
        for(int i = 0; i < blockSize; i++)
        {
            evad->expData[i] = (evad->expData[2*i + 1] + evad->expData[2*i])*0.5;
        }
    }
    
    logTimeExp = (float*)malloc(sizeof(float)*loglogsize);
    logValueExp = (float*)malloc(sizeof(float)*loglogsize);
    
    for(int i = 0; i < loglogsize; i++)
    {
        logTimeExp[i] = log(evad->timeExp[i]);
        logValueExp[i] =  log(evad->valueExp[i]);
    }
    
    float exponent = CompLogLogOfExponent(logTimeExp, logValueExp, loglogsize);
    
    free(logTimeExp);
    free(logValueExp);
    
    //evad->exponentV = exponent;
    
    
    biasExponent = 4*((exponent-0.5)*(exponent-0.5));
    
    
    return biasExponent;
}



float CompLogLogOfExponent(float * logTimeExp, float * logValueExp, int loglogsize)
{
    
    float logTimeSample = 0;
    float ls = loglogsize;
    float * TT;
    float * VV;
    float TT_s, TV_s, T_s, V_s;
    float slope, prev_slope;
    
    TT_s = 0;
    TV_s = 0;
    T_s = 0;
    V_s = 0;
    
    TT = (float*)malloc(sizeof(float)*(loglogsize));
    VV = (float*)malloc(sizeof(float)*(loglogsize));
    
    for(int i = 0; i < loglogsize; i++)
    {
        logTimeSample = logTimeExp[i];
        TT[i] = logTimeSample * logTimeSample;
        VV[i] = logTimeSample * logValueExp[i];
        TT_s = TT_s + TT[i];
        TV_s = TV_s + VV[i];
        T_s = T_s + logTimeExp[i];
        V_s = V_s + logValueExp[i];
    }
    prev_slope = (ls * TV_s - (T_s * V_s));
    slope = prev_slope / (ls * TT_s - (T_s * T_s));
    
    free(TT);
    free(VV);
    
    return slope;
    
}






















