//
//  DAFX.h
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-11-16.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//

#ifndef __Plotagon_RecordVoice__DAFX__
#define __Plotagon_RecordVoice__DAFX__

#include <stdio.h>


typedef struct DAFX_st {
    

    int MAX_DELAY;
    float *delay_line_dafx;
    int lengthOfDelayLine;
    int delayInSamples;
    int sampleFrameCounter;
    
    int dafxAlgoNbr;

    
}DAFX_t;


DAFX_t* echoGenereation_init(int fs, int framesize, float delayInSec, int effectNbrOn);
void echoGenereation_destroy(DAFX_t *dafx_echo);
void echoGenereation(DAFX_t *dafx_echo, float *buffer, int lengthOfBuffer, int fs, float fbStrength);

#endif /* defined(__Plotagon_RecordVoice__DAFX__) */
