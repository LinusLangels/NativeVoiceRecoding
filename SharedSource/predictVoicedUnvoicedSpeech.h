//
//  predictVoicedUnvoicedSpeech.h
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-10-05.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//

#ifndef __Plotagon_RecordVoice__predictVoicedUnvoicedSpeech__
#define __Plotagon_RecordVoice__predictVoicedUnvoicedSpeech__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define M_PI 3.1415926536

#define POSSIGNTARGET(a,b,target) (((a) >= target) && ((b) < target)? 1 : 0)
#define NEGSIGNTARGET(a,b,target) (((a) < target) && ((b) >= target)? 1 : 0)


typedef struct voicedVsUnvoiced_st {
    
 
    int frameCount;
    int avgSumZeroCrossingRate;
    float avgZeroCrossingRate;
    int currentZeroCrossingRate;
    float Coefficients;
    int voicedVsUnvoiced;
    
    float onepointDFTat500;
    float onepointDFTat3000;
    float onepointDFTat4000;

    
}voicedVsUnvoiced_t;

int voiceVsUnVoiced_exec(voicedVsUnvoiced_t *preVoice, float *lpc_parameters);
int zeroCrossingRate(float *frambuffer, int framelength, float target);
voicedVsUnvoiced_t* voiceVsUnVoiced_init(void);
void voiceVsUnVoiced_destroy(voicedVsUnvoiced_t *preVoice);
float slidingWindowAvgzeroCrossingRate(int currentZeroCrossingRate, voicedVsUnvoiced_t *preVoice);
float amplitudeAtFreqBin(int numSamples, int givenFreq, int Fs, float* frame);
void CalculateHurst(float * frame, int item, voicedVsUnvoiced_t *preVoice);
float CalculateFirstOrderHurstCoeff(float * logx, float * coeffs, int points);
float STD(float * data_p, int points);

#endif /* defined(__Plotagon_RecordVoice__predictVoicedUnvoicedSpeech__) */
