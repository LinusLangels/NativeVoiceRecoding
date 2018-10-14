//
//  predictVoicedUnvoicedSpeech.c
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-10-05.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//

#include "predictVoicedUnvoicedSpeech.h"


voicedVsUnvoiced_t* voiceVsUnVoiced_init(void)
{

    voicedVsUnvoiced_t *preVoice = (voicedVsUnvoiced_t *)malloc(sizeof(voicedVsUnvoiced_t));
    
    if(preVoice == NULL)
    {
        return NULL;
    }

    preVoice->avgSumZeroCrossingRate = 0;
    preVoice->frameCount = 1;
    
    preVoice->Coefficients = 0;
    
    preVoice->voicedVsUnvoiced = -1;
    
    return preVoice;
}


void voiceVsUnVoiced_destroy(voicedVsUnvoiced_t *preVoice)
{
    free(preVoice);
}

int voiceVsUnVoiced_exec(voicedVsUnvoiced_t *preVoice, float *lpc_parameters)
{
    int i;
    float abssum = 0;
    
    preVoice->voicedVsUnvoiced = -1; //set it to niether voice or un-voiced
  

    for(i=1;i<7;i++)
    {
        abssum = abssum + fabsf(lpc_parameters[i]);
    }
    if((abssum > 2) || (lpc_parameters[1] < -1.1))
    {
        //TBD - what?
    }
    if((lpc_parameters[1] * preVoice->Coefficients) < 0.4)
    {
        //TBD - probably very un-voiced
    }
    
    return preVoice->voicedVsUnvoiced;
}


/*
 * Calculate zero crossing rate per 10 ms, as an indication of high freq consonants
 * and un-voiced vs. voiced speech
 *
 */
int zeroCrossingRate(float *frame, int framelength, float target)
{
    int i;
    int diffsign;
    int avgZeroCrossing=0;
    
    diffsign = 0;
    for(i=0;i<framelength-1;i++)
    {
        diffsign = POSSIGNTARGET(frame[i],frame[i+1], 0) + NEGSIGNTARGET(frame[i],frame[i+1], 0);
        avgZeroCrossing = avgZeroCrossing + diffsign;
    }
    
    avgZeroCrossing = (int) ((float) (avgZeroCrossing) / 4);
    
    return avgZeroCrossing;
    
    
}

/*
 * Calculate a sliding window average update of zeroCrossingRate,
 * i.e. estimation and detection will be better by time.
 *
 */
float slidingWindowAvgzeroCrossingRate(int currentZeroCrossingRate, voicedVsUnvoiced_t *preVoice)
{
 
    float avgNormZeroCrossingRate;
    preVoice->avgSumZeroCrossingRate = preVoice->avgSumZeroCrossingRate + currentZeroCrossingRate;
    
    avgNormZeroCrossingRate = (((float) preVoice->avgSumZeroCrossingRate) / preVoice->frameCount);
    preVoice->frameCount++;
    
    return avgNormZeroCrossingRate;
}

/*
 * Calculate the strength at given freq (an one-point DFT)
 *
 */
float amplitudeAtFreqBin(int numSamples, int givenFreq, int Fs, float* frame)
{
    int     k,i;
    float   floatnumSamples;
    float   omega,sine,cosine,coeff,q0,q1,q2,real,imag;
    float   result;
    
    
    floatnumSamples = (float) numSamples;
    k = (int) (0.5 + ((floatnumSamples * givenFreq) / Fs));
    omega = (2.0 * M_PI * k) / floatnumSamples;
    sine = sin(omega);
    cosine = cos(omega);
    coeff = 2.0 * cosine;
    q0=0;
    q1=0;
    q2=0;
    
    for(i=0; i<numSamples; i++)
    {
        q0 = coeff * q1 - q2 + frame[i];
        q2 = q1;
        q1 = q0;
    }
    real = (q1 - q2 * cosine);
    imag = (q2 * sine);
    result = sqrtf(real*real + imag*imag);
    
    return result;
    
    
}

/*
 * Calculate Hurst Coeff, as an indication together with lpc parameters 
 * for voice/un-voiced speech when AZC is not enough as an indication
 *
 */

void CalculateHurst(float * frame, int item, voicedVsUnvoiced_t *preVoice)
{
    float * x_vals;
    float * y_vals;
    float * data;
    int index = 0;
    int binsize = 1;
    int npoints;
    float * logx;
    float * logy;
    int i;
    
    
    x_vals = (float*)malloc(sizeof(float) * (item));
    y_vals = (float*)malloc(sizeof(float) * (item));
    data = (float*)malloc(sizeof(float) * (item));
    
    for(i=0;i<item;i++)
    {
        x_vals[i]=0;
        y_vals[i]=0;
        data[i]=frame[i];
    }

    npoints = item;
    
    while( npoints > 4)
    {
        float y = STD(data, npoints);
        
        x_vals[index] = binsize;
        y_vals[index] = binsize * y;
        index++;
        binsize = binsize * 2;
        npoints = npoints / 2 ;
        
        for(int i = 0; i < npoints; i++)
        {
            data[i] = (data[2*i + 1] + data[2*i])*0.5;
        }
    }
    
    logx = (float*)malloc(sizeof(float)*index);
    logy = (float*)malloc(sizeof(float)*index);
    
    for(int i = 0; i < index; i++)
    {
        logx[i] = log(x_vals[i]);
        logy[i] = log(y_vals[i]);
    }

 
    preVoice->Coefficients = CalculateFirstOrderHurstCoeff(logx, logy, index);
    
    free(logx);
    free(logy);
    free(x_vals);
    free(y_vals);
    free(data);
}



float CalculateFirstOrderHurstCoeff(float * logx, float * coeffs, int points)
{

    int ignorevalue = 0;
    float startvalue = 0;
    
    float x_value = ignorevalue + startvalue;
    float l = points - ignorevalue;
    float * xx;
    float * xy;
    float xx_sum = 0;
    float xy_sum = 0;
    float x_sum = 0;
    float y_sum = 0;
    float b;
    float b_temp;
    
    
    xx = (float*)malloc(sizeof(float)*(points-ignorevalue));
    xy = (float*)malloc(sizeof(float)*(points-ignorevalue));
    
    for(int i = ignorevalue; i < points; i++)
    {
        x_value = logx[i];
        xx[i-ignorevalue] = x_value * x_value;
        xy[i-ignorevalue] = x_value * coeffs[i];

    }
    
    
    for(int i = 0; i < points - ignorevalue; i++)
    {
        xx_sum = xx_sum + xx[i];
        xy_sum = xy_sum + xy[i];
        x_sum = x_sum + logx[i];
        
        y_sum = y_sum + coeffs[i];
    }
    
    b_temp = (l * xy_sum - (x_sum * y_sum));
    b = b_temp / (l * xx_sum - (x_sum * x_sum));
    
    free(xx);
    free(xy);
    
    return b;
    
}

float STD(float * data_p, int points)
{
    int m = 0;
    float tmp = 0;
    
    for(int i = 0; i < points; i++)
    {
        m = m + data_p[i];
    }
    
    m = m / points;
    
    for(int i = 0; i < points; i++)
    {
        tmp = tmp + ((data_p[i]-m)*(data_p[i]-m));
    }
    
    tmp = tmp/((float)points-1);
    tmp = sqrtf(tmp);
    
    return tmp;
    
}

