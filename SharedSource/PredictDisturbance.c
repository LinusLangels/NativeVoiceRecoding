//
//  PredictDisturbance.c
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-10-13.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//

#include "PredictDisturbance.h"


predictDisturbance_t* predictDisturbance_init(void)
{
    
    predictDisturbance_t *preDist = (predictDisturbance_t *)malloc(sizeof(predictDisturbance_t));
    
    if(preDist == NULL)
    {
        return NULL;
    }
    
    preDist->movingAverage = 1;
    preDist->alfaFilterCoeff = 0.175;
    preDist->raiseOfSignalTarget = 3.0f;
    preDist->candidateOfSpike = 0;
    preDist->concealSpikeWithNoise = 1;
    
    return preDist;
}


void predictDisturbance_destroy(predictDisturbance_t *preDist)
{
    free(preDist);
}

predictDisturbance_t* predictDisturbance_exec(predictDisturbance_t *preDist, float framebufferEnergy, float *frame, float latestBackgroundEnergy)
{
    
    preDist->movingAverage = (1-preDist->alfaFilterCoeff)*preDist->movingAverage + preDist->alfaFilterCoeff*framebufferEnergy;
    
    if((fabsf(preDist->movingAverage - framebufferEnergy) / preDist->movingAverage) > preDist->raiseOfSignalTarget)
    {
        preDist->candidateOfSpike = 1;
    }
    else
    {
        preDist->candidateOfSpike = 0;
    }
    
    return preDist;
}