//
//  CalibrateAudioData.c
//  MicrophoneRecorder
//
//  Created by John Philipsson on 2016-01-14.
//  Copyright © 2016 John Philipsson. All rights reserved.
//

#define _CRT_SECURE_NO_DEPRECATE //Allow fopen on windows.

#include "CalibrateAudioData.h"

void CalibrateAudio(CALIBRATION_t *calibrate, float *buffer, int avgMeanSize, int lengthOfBuffer)
{
    if(avgMeanSize < 1)
    {
        calibrate->maxAbsMean = 1;
        return;
    }
    if((avgMeanSize*2) > lengthOfBuffer)
    {
        return;
    }
    
    float maxAbsMeanForCurrentBuffer = CalibrationEstimation(buffer,avgMeanSize,lengthOfBuffer);
    if(maxAbsMeanForCurrentBuffer > calibrate->maxAbsMean)
    {
        calibrate->maxAbsMean = maxAbsMeanForCurrentBuffer;
    }
    
    
    //to avoid extreme amplification (in the calibration process) if signal is close to zero
    if(calibrate->maxAbsMean < 0.1)
    {
        calibrate->maxAbsMean = 0.1;
    }
        
}

float CalibrationEstimation(float *buffer, int avgMeanSize, int lengthOfBuffer)
{
    
    int i,j,k;
    float blockSum;
    float AbsMean;
    float maxAbsMinInBuffer;
    

    maxAbsMinInBuffer = -1;
    int blockLength = (int) floor(((float)lengthOfBuffer) / ((float)avgMeanSize));
    
    k=0;
    AbsMean = 0;
    for(i=0;i<blockLength;i++)
    {
        blockSum=0;
        for(j=0;j<avgMeanSize;j++)
        {
            blockSum = blockSum + fabs(buffer[k]);
            k++;
        }
        AbsMean = blockSum/avgMeanSize;
        if(AbsMean > maxAbsMinInBuffer)
        {
            maxAbsMinInBuffer = AbsMean;
        }
    }

    return maxAbsMinInBuffer;
}





