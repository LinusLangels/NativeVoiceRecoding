//
//  DecimateAudioData.h
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-10-06.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//

#ifndef __Plotagon_RecordVoice__DecimateAudioData__
#define __Plotagon_RecordVoice__DecimateAudioData__

#include <stdio.h>

static float h_30[] = {
    -0.001204,-0.002053,-0.002080,0.000000,0.004765,0.009896,0.009978,
    -0.000000,-0.018964,-0.036293,-0.034762,0.000000,0.068632,0.153266,
    0.223458,0.250720,0.223458,0.153266,0.068632,0.000000,-0.034762,
    -0.036293,-0.018964,-0.000000,0.009978,0.009896,0.004765,0.000000,
    -0.002080,-0.002053,-0.001204};

typedef struct decimateaudio_st {
    
    int framesize;
    int filter_length_dec;
    float * filter_taps_dec;
    float * delay_dec;
    int samplerate_R;
    
    float *temp_frame;
    
    int decimated_fs;
    int decimatedframesize;
    
}decimateaudio_t;


decimateaudio_t * decimateaudio_init(int full_fs, int framesize);
void decimateaudio_destroy(decimateaudio_t * dec);
int decimate_signal(decimateaudio_t * dec,float *frame, int framesize,int decimate_factor_R);
void lowpass_filter_signal(decimateaudio_t * dec, float *frame, int framesize, float *filter_taps);

#endif /* defined(__Plotagon_RecordVoice__DecimateAudioData__) */
