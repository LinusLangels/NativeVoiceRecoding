//
//  EnhancedTalkVsNoiseDetection.h
//  MicrophoneRecorder
//
//  Created by John Philipsson on 2016-01-12.
//  Copyright © 2016 John Philipsson. All rights reserved.
//

#ifndef EnhancedTalkVsNoiseDetection_h
#define EnhancedTalkVsNoiseDetection_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct eVAD_st {
    
    int robustnessFactor;
    float *eVAD_buffer;
    float *MA_buffer;
    
    float *timeExp;
    float *valueExp;
    float *expData;
    
    float weightingTarget;
    
    float exponentV;
    
    
    
}eVAD_t;


eVAD_t* ehancedVAD_init(int fs, int framesize, int robustnessFactor, float weightingTarget);
void ehancedVAD_destroy(eVAD_t *evad);
void ehancedVAD_exec(eVAD_t *evad, float *buffer, int lengthOfBuffer, int fs, int robustnessFactor);

float EstimateSpectralExponent(eVAD_t *evad, float * frame, int framelength);
float CompLogLogOfExponent(float * logTimeExp, float * logValueExp, int loglogsize);

void WeightBufferSamples(eVAD_t *evad, int framelength, float exponentValue);

#endif /* EnhancedTalkVsNoiseDetection_h */
