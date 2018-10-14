//
//  DecimateAudioData.c
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-10-06.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//

#include "DecimateAudioData.h"
#include <math.h>
#include <stdlib.h>

decimateaudio_t * decimateaudio_init(int full_fs, int framesize)
{
    int i;
    
    decimateaudio_t * dec = (decimateaudio_t *)malloc(sizeof(decimateaudio_t));
    
    if(dec == NULL)
    {
        return NULL;
    }
    dec->framesize = framesize;
    
    dec->filter_length_dec=31;
    dec->samplerate_R=4;
    dec->decimated_fs = (int) ((float) full_fs / dec->samplerate_R);
    
    dec->decimatedframesize = (framesize / dec->samplerate_R);
    
    //printf("%d\n",dec->decimatedframesize);
    
    dec->filter_taps_dec = (float *)malloc(dec->filter_length_dec * sizeof(float));
    dec->delay_dec = (float *)malloc(dec->filter_length_dec* sizeof(float));
    dec->temp_frame = (float *)malloc(dec->decimatedframesize*sizeof(float));                               //framesize assumed to be 128 (in 11025 Hz mode) before i.e 512 / 4;
    
    // init decimate and interpolation delayline with zeros
    for(i=0;i<dec->filter_length_dec;i++)
    {
        dec->delay_dec[i] = 0;
        dec->filter_taps_dec[i] = h_30[i];
    }

 
    
    return dec;
}

void decimateaudio_destroy(decimateaudio_t * dec)
{
    
    free(dec->delay_dec);
    free(dec->filter_taps_dec);
    free(dec->temp_frame);
    free(dec);
    
}


void lowpass_filter_signal(decimateaudio_t * dec, float *frame, int framesize, float *filter_taps)
{
    int j, i;
    double y = 0;
    double tmp;
    
    for(j=0;j<framesize;j++)
    {
        y = 0;
        for (i = dec->filter_length_dec-1; i > 0 ; i--)
        {
            dec->delay_dec[i] = dec->delay_dec[i-1];
            y = y + dec->delay_dec[i] * filter_taps[i];
        }
        dec->delay_dec[0] = frame[j];
        tmp = (y + dec->delay_dec[0] * filter_taps[0]);
        frame[j] = tmp;
    }
    
}

int decimate_signal(decimateaudio_t * dec, float *frame, int framesize, int decimate_factor_R)
{
    
    int decimated_framesize;
    int i;
    
    if((framesize == dec->framesize) && (decimate_factor_R == 4))
    {
        decimated_framesize=dec->decimatedframesize;
        
        //call lowpass filtering for factor 6
        if(decimate_factor_R == 4)
        {
            lowpass_filter_signal(dec, frame, framesize, dec->filter_taps_dec);
        }
        //then do decimation
        for(i=0;i<decimated_framesize;i++)
        {
            frame[i]=frame[i*decimate_factor_R];
        }
    }
    else
    {
        //not possible to decimate i.e. return same framesize length
        return framesize;
    }
    
    //decimation OK! return 160 size framesize
    return decimated_framesize;
}