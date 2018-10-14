//
//  VoiceRecordingUtils.c
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-09-30.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//
#define _CRT_SECURE_NO_WARNINGS

#include "VoiceRecordingUtils.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

voiceRec_t* CalculatePhoenems_init(int fs, int bufferlength, bool fullSearchModeOn, int effectNbrOn, bool noiseDetection)
{
    
    voiceRec_t *voiceRec = (voiceRec_t *)malloc(sizeof(voiceRec_t));
    
    if(voiceRec == NULL)
    {
        return NULL;
    }
    
    
    voiceRec->FRAMESIZE = bufferlength;
    voiceRec->fs = fs;
    
    voiceRec->phonemeStartTime = 0.0;
    voiceRec->phonemeStopTime = 0.0;
    voiceRec->phonemeFrameTimeDuration = (float) voiceRec->FRAMESIZE/ (float) voiceRec->fs;
    voiceRec->frameCount = 0;
    
    voiceRec->run_CalcSpeechEnergy = true;
    voiceRec->run_PredictDisturbance = true;
    voiceRec->run_predictVoicedUnvoicedSpeech = true;
    voiceRec->run_Vowel_Identification = true;
    voiceRec->run_OnePointDFT = true;
    voiceRec->run_EnhancedVoiceVsNoiseDetection = noiseDetection;
    
    
    voiceRec->run_halfPhomemeSpeedUpdate = true; // This one is platform specific, needs to be moved out to Unity interface
    
    voiceRec->lowComplexityMode = 0;
    
    if(!fullSearchModeOn)
    {
        voiceRec->lowComplexityMode = 1;
        voiceRec->run_PredictDisturbance = false; // this turn off the extra check for spikes in the signal
        voiceRec->run_predictVoicedUnvoicedSpeech = false; //this turn off, hurst algo, and all search for different voiced and unvoiced consonants
        voiceRec->run_OnePointDFT = false; // this turn of the extra amplitude check of freqs
        voiceRec->run_EnhancedVoiceVsNoiseDetection = false; // turn off the extra robust VAD, i.e. runs then only the normal VAD (Voice Activity Detector)
    }
    
    
    voiceRec->vowelString = "\0";
    voiceRec->voicedString = "\0";
    voiceRec->unVoicedString = "\0";
    voiceRec->phoneupdate = 0;
    
    
    voiceRec->signalE = calcSpeechEnergy_init(voiceRec->FRAMESIZE/4,voiceRec->FRAMESIZE); //short buffer 160 must be a even multiple of the long 640 i.e. (4*160 = 640)
    voiceRec->preVoice = voiceVsUnVoiced_init();
    voiceRec->dec = decimateaudio_init(voiceRec->fs,voiceRec->FRAMESIZE);
    voiceRec->vow = vowelIdentification_init(voiceRec->fs,voiceRec->FRAMESIZE/4);  //framsize only 145 thus decimated from 580 -> 145
    voiceRec->preDist = predictDisturbance_init();
    
    // init of enhanced voice vs. noise detection, robustnessFactor = 150 (default);
    // init of enhanced voice vs. noise detection, weightingTarget = 0.9 (default);
    voiceRec->evad = ehancedVAD_init(voiceRec->fs,voiceRec->FRAMESIZE/4, 150, 0.85);
    
    voiceRec->phonProb = calcPhonOrdProb_init(5);
    
    voiceRec->audioFXon=0;
    
    
    if(effectNbrOn > 0)
    {
        //assume realtime system atleast gives a buffer shorter than 10000 samples, otherwise, it can not be runned, to be checked
        //You can never know for sure the buffer size given, keep a margin
        voiceRec->audioFXon=effectNbrOn;
        voiceRec->dafx = echoGenereation_init(fs,10000,0.7,effectNbrOn);
    }
    
    return voiceRec;
}

void CalculatePhoenems_destroy(voiceRec_t *voiceRec, int effectNbrOn)
{
    calcSpeechEnergy_destroy(voiceRec->signalE);
    vowelIdentification_destroy(voiceRec->vow);
    voiceVsUnVoiced_destroy(voiceRec->preVoice);
    decimateaudio_destroy(voiceRec->dec);
    predictDisturbance_destroy(voiceRec->preDist);
    
    ehancedVAD_destroy(voiceRec->evad);  // destroy the enganced Voice detection
    
    calcPhonOrdProb_destroy(voiceRec->phonProb);
    
    
    if(effectNbrOn > 0)
    {
        echoGenereation_destroy(voiceRec->dafx);
    }
    
    free(voiceRec);
}

voiceRec_t* CalculatePhoenems(float *frame, signalEnergy_t *signalE, vowel_t *vow, voiceRec_t *voiceRec, voicedVsUnvoiced_t *preVoice, decimateaudio_t *dec, predictDisturbance_t *preDist, eVAD_t *evad)
{
    
    int i;
    int index_ptr;
    
    if(voiceRec->run_EnhancedVoiceVsNoiseDetection == true)
    {
        
        // manupulate audio data on a copy of input audio data and then
        //use the normal VAD based on the modified audio data (as input) to calculate signal energies and detect speech
        index_ptr=0;
        for(i=0;i<signalE->sizeOfLongEnergymemory;i++)
        {
            
            ehancedVAD_exec(evad, frame+index_ptr, signalE->shortFrameSize, voiceRec->fs,evad->robustnessFactor);
            
            signalE = calcSpeechEnergy_exec(signalE,evad->eVAD_buffer);
            index_ptr = index_ptr + signalE->shortFrameSize;
        }
    }
    else
    {
        //Calculate signal energies and detect speech
        index_ptr=0;
        for(i=0;i<signalE->sizeOfLongEnergymemory;i++)
        {
            signalE = calcSpeechEnergy_exec(signalE,frame+index_ptr);
            index_ptr = index_ptr + signalE->shortFrameSize;
        }
    }
    
    
    
    //run AZC algos
    preVoice->currentZeroCrossingRate = zeroCrossingRate(frame,voiceRec->FRAMESIZE,0);
    preVoice->avgZeroCrossingRate = slidingWindowAvgzeroCrossingRate(preVoice->currentZeroCrossingRate,preVoice);
    
    if(voiceRec->run_OnePointDFT == true)
    {
        //run 1-point DFT @ freq
        preVoice->onepointDFTat500 = amplitudeAtFreqBin(voiceRec->FRAMESIZE,500,voiceRec->fs,frame);
        preVoice->onepointDFTat3000 = amplitudeAtFreqBin(voiceRec->FRAMESIZE,3000,voiceRec->fs,frame);
        preVoice->onepointDFTat4000 = amplitudeAtFreqBin(voiceRec->FRAMESIZE,4000,voiceRec->fs,frame);
    }
    
    if(voiceRec->run_PredictDisturbance == true)
    {
        //run test if spikes
        preDist=predictDisturbance_exec(preDist,signalE->longEnergy, frame, signalE->BackgroundStatic);
    }
    else{
        preDist->candidateOfSpike = 0;
    }
    
    if(voiceRec->run_predictVoicedUnvoicedSpeech == true)
    {
        //run hurst
        CalculateHurst(frame,voiceRec->FRAMESIZE,preVoice);
    }
    
    //decimate signal for the vowel calculation
    int framelengthdecimated = decimate_signal(dec,frame,voiceRec->FRAMESIZE,dec->samplerate_R);
    
    //if speech detected, and no spike, and that signal have been correct decimated, check if it contains any vowel, and calculate that vowel
    //if no, vowel, check consonants sound, voiced and un-voiced
    if((signalE->speechdetected == true) && (framelengthdecimated == dec->decimatedframesize) && (preDist->candidateOfSpike == 0))
    {
        
        vow=vowelIdentification_exec(vow,frame,dec->decimated_fs);
        
        if(vow->vowelExists == true)
        {
            strcpy(voiceRec->phoneme,vow->vowelString);
            //printf("SPEECH DETECTED, vowel string is %s\n",voiceRec->phoneme);
        }
        else{
            
            if(voiceRec->run_predictVoicedUnvoicedSpeech == true)
            {
                // if voiced, then try to map closest consonant
                if(preVoice->Coefficients > 0.5)
                {
                    voiceRec->voicedString = calcVoicedPhoneme2(vow->ar_coeff_out,preVoice->currentZeroCrossingRate,preVoice->Coefficients, preVoice);
                    strcpy(voiceRec->phoneme,voiceRec->voicedString);
                    //printf("SPEECH DETECTED, voiced consonant string is %s\n",voiceRec->phoneme);
                }
                else // if un-voiced, then try to map closest consonant
                {
                    voiceRec->unVoicedString = calcUnVoicedPhoneme2(vow->ar_coeff_out,preVoice->currentZeroCrossingRate,preVoice->Coefficients, preVoice);
                    strcpy(voiceRec->phoneme,voiceRec->unVoicedString);
                    //printf("SPEECH DETECTED, Un-voiced consonant string is %s\n",voiceRec->phoneme);
                }
            }
            else{
                strcpy(voiceRec->phoneme,"t\0");
                //printf("SPEECH DETECTED, No consonant search, consonant is set to: %s\n",voiceRec->phoneme);
            }
            
        }
    }
    else
    {
        
        //print here if it is a spike
        if(preDist->candidateOfSpike == 1)
        {
            //printf("SPIKE\n");
        }
        // No Speech, it is silence
        // or something went wrong in buffer handling, i.e. decimate return buffer != 145 samples
        vow->vowelString = "sil\0";
        strcpy(voiceRec->phoneme,vow->vowelString);
        
        //printf("SILENCE\n");
        
    }
    
    if(voiceRec->run_halfPhomemeSpeedUpdate == true)
    {
        if(voiceRec->phoneupdate == 1)
        {
            strcpy(voiceRec->phoneme,voiceRec->prev_phoneme);
            voiceRec->phoneupdate = 0;
        }
        else{
            voiceRec->phoneupdate = 1;
        }
    }
    
    //as a last step, check the probability for the found phoneme
    calcPhonOrdProbExec(voiceRec->phonProb,voiceRec->phoneme,preVoice->currentZeroCrossingRate);
    
    //set phoneme start- and stop-time
    voiceRec->phonemeStartTime = voiceRec->frameCount*voiceRec->phonemeFrameTimeDuration;
    voiceRec->phonemeStopTime = voiceRec->phonemeStartTime + voiceRec->phonemeFrameTimeDuration;
    
    
    strcpy(voiceRec->prev_phoneme,voiceRec->phoneme);
    
    
    //update frame count
    voiceRec->frameCount++;
    
    
    return voiceRec;
}