//
//  CalculateUnVoicedPhoneme.h
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-10-15.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//

#ifndef __Plotagon_RecordVoice__CalculateUnVoicedPhoneme__
#define __Plotagon_RecordVoice__CalculateUnVoicedPhoneme__

#include <stdio.h>
#include <math.h>
#include "predictVoicedUnvoicedSpeech.h"

char *calcUnVoicedPhoneme(float *lpc_parameters, int azc, float hurst, voicedVsUnvoiced_t *preVoice);
char *calcUnVoicedPhoneme2(float *lpc_parameters, int azc, float hurst, voicedVsUnvoiced_t *preVoice);

#endif /* defined(__Plotagon_RecordVoice__CalculateUnVoicedPhoneme__) */
