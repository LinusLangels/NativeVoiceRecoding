//
//  VoiceRecordingUtils.h
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-09-30.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//

#ifndef __Plotagon_RecordVoice__VoiceRecordingUtils__
#define __Plotagon_RecordVoice__VoiceRecordingUtils__

#include <stdio.h>
#include <stdbool.h>
#include "CalcSpeechEnergy.h"
#include "Vowel_Identification.h"
#include "predictVoicedUnvoicedSpeech.h"
#include "DecimateAudioData.h"
#include "PredictDisturbance.h"
#include "CalculateVoicedPhoneme.h"
#include "CalculateUnVoicedPhoneme.h"
#include "DAFX.h"
#include "EnhancedTalkVsNoiseDetection.h"
#include "CalcPhonemeOrderProbability.h"


typedef struct voiceRec_st {
    
    int FRAMESIZE;
    int fs;
    int frameCount;
    float phonemeStartTime;
    float phonemeStopTime;
    float phonemeFrameTimeDuration;
    char phoneme[5];
    
    char prev_phoneme[5];
    int phoneupdate;
    
    
    char *vowelString;
    char *unVoicedString;
    char *voicedString;
    
    signalEnergy_t *signalE;
    voicedVsUnvoiced_t *preVoice;
    decimateaudio_t *dec;
    vowel_t *vow;
    predictDisturbance_t *preDist;
    DAFX_t *dafx;
    eVAD_t *evad;
    PhonemeProb_t *phonProb;
    
    int lowComplexityMode;
    
    //to be used to set algo ON/OFF
    bool run_CalcSpeechEnergy;
    bool run_Vowel_Identification;
    bool run_predictVoicedUnvoicedSpeech;
    bool run_PredictDisturbance;
    bool run_OnePointDFT;
    bool run_EnhancedVoiceVsNoiseDetection;
    
    bool run_halfPhomemeSpeedUpdate;
    
    int audioFXon;
    
}voiceRec_t;

#ifdef __cplusplus
extern "C"
{
#endif
	voiceRec_t* CalculatePhoenems(float *frame, signalEnergy_t *signalE, vowel_t *vow, voiceRec_t *voiceRec, voicedVsUnvoiced_t *preVoice, decimateaudio_t *dec, predictDisturbance_t *preDist, eVAD_t *evad);
	voiceRec_t* CalculatePhoenems_init(int fs, int bufferlength, bool fullSearchModeO, int effectNbrOn, bool noiseDetection);
	void CalculatePhoenems_destroy(voiceRec_t *voiceRec, int effectNbrOn);
#ifdef __cplusplus
}
#endif

#endif /* defined(__Plotagon_RecordVoice__VoiceRecordingUtils__) */
