//
//  PredictDisturbance.h
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-10-13.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//

#ifndef __Plotagon_RecordVoice__PredictDisturbance__
#define __Plotagon_RecordVoice__PredictDisturbance__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct predictDisturbance_st {
    
    float movingAverage;
    float alfaFilterCoeff;
    float raiseOfSignalTarget;
    int candidateOfSpike;
    int concealSpikeWithNoise;

    
    
}predictDisturbance_t;

predictDisturbance_t* predictDisturbance_init(void);
predictDisturbance_t* predictDisturbance_exec(predictDisturbance_t *preDist, float framebufferEnergy, float *frame, float latestBackgroundEnergy);
void predictDisturbance_destroy(predictDisturbance_t *preDist);


#endif /* defined(__Plotagon_RecordVoice__PredictDisturbance__) */
