#pragma once
//
//  from file: VoiceRecordingUtils.h
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-09-30.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//


#ifndef _DLL_PHON_H_
#define _DLL_PHON_H_


#ifdef DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif


extern "C"
{

#include <stdio.h>
#include <stdbool.h>
#include "CalcSpeechEnergy.h"
#include "Vowel_Identification.h"
#include "predictVoicedUnvoicedSpeech.h"
#include "DecimateAudioData.h"
#include "PredictDisturbance.h"
#include "CalculateVoicedPhoneme.h"
#include "CalculateUnVoicedPhoneme.h"



	

typedef struct voiceRec_st {

	int FRAMESIZE;
	int fs;
	int frameCount;
	float phonemeStartTime;
	float phonemeStopTime;
	float phonemeFrameTimeDuration;
	char phoneme[5];

	char *vowelString;
	char *unVoicedString;
	char *voicedString;

	signalEnergy_t *signalE;
	voicedVsUnvoiced_t *preVoice;
	decimateaudio_t *dec;
	vowel_t *vow;
	predictDisturbance_t *preDist;

	int lowComplexityMode;

	//to be used to set algo ON/OFF
	bool run_CalcSpeechEnergy;
	bool run_Vowel_Identification;
	bool run_predictVoicedUnvoicedSpeech;
	bool run_PredictDisturbance;
	bool run_OnePointDFT;

}voiceRec_t;


DECLDIR voiceRec_t* CalculatePhoenems(float *frame, signalEnergy_t *signalE, vowel_t *vow, voiceRec_t *voiceRec, voicedVsUnvoiced_t *preVoice, decimateaudio_t *dec, predictDisturbance_t *preDist);
DECLDIR voiceRec_t* CalculatePhoenems_init(int fs, int bufferlength, bool fullSearchModeOn);
DECLDIR void CalculatePhoenems_destroy(voiceRec_t *voiceRec);





}

#endif//_DLL_PHON_H_