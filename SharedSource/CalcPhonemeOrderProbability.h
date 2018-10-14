#define _CRT_SECURE_NO_WARNINGS //Allow strcpy on windows.

//
//  CalcPhonemeOrderProbability.h
//  MicrophoneRecorder
//
//  Created by John Philipsson on 2016-01-28.
//  Copyright © 2016 John Philipsson. All rights reserved.
//

#ifndef CalcPhonemeOrderProbability_h
#define CalcPhonemeOrderProbability_h

#include <stdio.h>

#define PHONEME_LEN 5

typedef struct PhonemeProb_st {
    
    char **delaylinePhoneBuffer;
    int phonemeOrderBufferSize;
    char PHONEME_TABLE_PROB[23][3];
    int index_compared_ptr;
    
}PhonemeProb_t;


PhonemeProb_t* calcPhonOrdProb_init(int phonemeOrderBufferSize);
void calcPhonOrdProb_destroy(PhonemeProb_t *phonProb);
void calcPhonOrdProbExec(PhonemeProb_t *phonProb,char *phoneme, int zcr);

#endif /* CalcPhonemeOrderProbability_h */
