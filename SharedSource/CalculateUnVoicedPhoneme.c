//
//  CalculateUnVoicedPhoneme.c
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-10-15.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//

#include "CalculateUnVoicedPhoneme.h"


// these phonemes is unvoiced or similar
char UNVOICED_STRING_TABLE[5][3] = {"f\0", "k\0","p\0","s\0","t\0"};



/*
 * Calculate a voiced phoneme based on DFT, lpc_parameters (maybe), azc and hurst (all pre-calculated)
 *
 */
char *calcUnVoicedPhoneme2(float *lpc_parameters, int azc, float hurst, voicedVsUnvoiced_t *preVoice)
{
    
    
    float DFT1, DFT2, DFT3;
    
    DFT1 = preVoice->onepointDFTat500;
    DFT2 = preVoice->onepointDFTat3000;
    DFT3 = preVoice->onepointDFTat4000;
    
    if((4*azc) < 90)
    {
        return UNVOICED_STRING_TABLE[2];
    }
    if((4*azc) < 150)
    {
        return UNVOICED_STRING_TABLE[1];
    }
    if((DFT2 > (0.75*DFT1)) && (DFT1 > (0.75*DFT2)))
    {
        return UNVOICED_STRING_TABLE[3];
    }
    if(DFT2 > (1.5*DFT1))
    {
        return UNVOICED_STRING_TABLE[0];
    }
    if(DFT1 > (5*DFT3))
    {
        return UNVOICED_STRING_TABLE[4];
    }

    return UNVOICED_STRING_TABLE[0];
}


/*
 * Calculate a voiced phoneme based on lpc parameters, azc and hurst (all pre-calculated)
 *
 */
char *calcUnVoicedPhoneme(float *lpc_parameters, int azc, float hurst, voicedVsUnvoiced_t *preVoice)
{
    
    int j;
    float abssum = 0;
    

    if(hurst > 0.2)
    {
        if(azc < 25)
        {
            return UNVOICED_STRING_TABLE[2];
        }
        else
        {
            return UNVOICED_STRING_TABLE[1];
        }
    }
    else
    {
        for(j=1;j<7;j++)
        {
            abssum = abssum + fabsf(lpc_parameters[j]);
        }
        if(abssum < 0.75)
        {
            return UNVOICED_STRING_TABLE[0];
        }
        else
        {
            if(abssum > 1.5)
            {
                return UNVOICED_STRING_TABLE[3];
            }
            else
            {
                return UNVOICED_STRING_TABLE[4];
            }
        }
    }
    
    return UNVOICED_STRING_TABLE[2];
}