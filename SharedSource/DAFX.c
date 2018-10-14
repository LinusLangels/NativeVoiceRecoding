//
//  DAFX.c
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-11-16.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//

#include "DAFX.h"
#include <stdlib.h>


DAFX_t* echoGenereation_init(int fs, int framesize, float delayInSec, int effectNbrOn)
{
    int i;
    int sampleDelay = (int)(fs*delayInSec);
    
    DAFX_t *dafx_echo = (DAFX_t *)malloc(sizeof(DAFX_t));
    
    if(dafx_echo == NULL)
    {
        return NULL;
    }
    
    dafx_echo->lengthOfDelayLine = sampleDelay+framesize;
    dafx_echo->delayInSamples = sampleDelay;
    
    dafx_echo->MAX_DELAY = 3; //set max delay in sec.
    
    if(delayInSec > dafx_echo->MAX_DELAY) // do not allow delay larger than 3 sek.
    {
        delayInSec =  dafx_echo->MAX_DELAY;
    }
    
    dafx_echo->delay_line_dafx = (float *)malloc((dafx_echo->lengthOfDelayLine)* sizeof(float));
    
    for(i=0;i<dafx_echo->lengthOfDelayLine;i++)
    {
        dafx_echo->delay_line_dafx[i] = 0;
    
    }
    
    dafx_echo->sampleFrameCounter = 0;
    
    dafx_echo->dafxAlgoNbr = effectNbrOn;
    
    return dafx_echo;
}


void echoGenereation_destroy(DAFX_t *dafx_echo)
{
    
    free(dafx_echo->delay_line_dafx);
    free(dafx_echo);
    
}


void echoGenereation(DAFX_t *dafx_echo, float *buffer, int lengthOfBuffer, int fs, float fbStrength)
{
 
    int i;
    
    if(fbStrength < -0.99)  // do not allow feedback-loop that increase, => howling
        return;
    
    if(fbStrength > 0.99) // do not allow feedback-loop that increase, => howling
        return;
    

    dafx_echo->sampleFrameCounter++;
    
    //update delayline
    for(i=0;i<(dafx_echo->lengthOfDelayLine) - lengthOfBuffer;i++)
    {
        dafx_echo->delay_line_dafx[dafx_echo->lengthOfDelayLine - 1 - i] = dafx_echo->delay_line_dafx[dafx_echo->lengthOfDelayLine - 1 - i - lengthOfBuffer];
    }
    for(i=0;i<lengthOfBuffer;i++)
    {
        dafx_echo->delay_line_dafx[i] = buffer[lengthOfBuffer-1-i];
    }
    
    
    if(((dafx_echo->sampleFrameCounter)*lengthOfBuffer) > dafx_echo->delayInSamples)
    {
        for(i=0;i<lengthOfBuffer;i++)
        {
            dafx_echo->delay_line_dafx[i] = buffer[lengthOfBuffer-1-i] + fbStrength*(dafx_echo->delay_line_dafx[dafx_echo->delayInSamples+i]);
        }
      
    }
     
    
    for(i=0;i<lengthOfBuffer;i++)
    {
        buffer[i] = dafx_echo->delay_line_dafx[lengthOfBuffer-1-i];
    }
     
    
}