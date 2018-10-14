//
//  CalculateVoicedPhoneme.c
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-10-15.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//

#include "CalculateVoicedPhoneme.h"


// these phonemes is voiced or similar
char VOICED_STRING_TABLE[8][3] = {"b\0", "l\0","m\0","n\0","d\0","g\0","h\0","r\0"};




/*
 * Calculate a voiced phoneme based on DFT, lpc_parameters (maybe), azc and hurst (all pre-calculated)
 *
 */
char *calcVoicedPhoneme2(float *lpc_parameters, int azc, float hurst, voicedVsUnvoiced_t *preVoice)
{
    
    float DFT1, DFT2, DFT3;
    
    DFT1 = preVoice->onepointDFTat500;
    DFT2 = preVoice->onepointDFTat3000;
    DFT3 = preVoice->onepointDFTat4000;
    
    if((4*azc) > 25)
    {
        if(DFT1 > (30*DFT3))
        {
            return VOICED_STRING_TABLE[0];
        }
        if(DFT1 > (20*DFT3))
        {
            if((4*azc) < 60)
                return VOICED_STRING_TABLE[6];
            else
                return VOICED_STRING_TABLE[5];
                
        }
        if(DFT1 > (10*DFT3))
        {
            return VOICED_STRING_TABLE[7];
        }
        if(DFT1 > (5*DFT3))
        {
            return VOICED_STRING_TABLE[4];
        }
        if(DFT1 < (DFT3))
        {
            return VOICED_STRING_TABLE[5];
        }
        
        return VOICED_STRING_TABLE[1];
  
    }
    else
    {
        if(DFT1 > (25*DFT3))
        {
            if((4*azc) > 20)
                return VOICED_STRING_TABLE[0];
                
        }
        if(DFT1 > (7*DFT2))
        {
            if((4*azc) > 15)
                return VOICED_STRING_TABLE[1];
            else
                return VOICED_STRING_TABLE[3];
                
        }
        if(DFT1 > (5*DFT2))
        {
            if((4*azc) > 10)
                return VOICED_STRING_TABLE[3];
            else
                return VOICED_STRING_TABLE[2];
        }
        if(DFT1 > (4*DFT2))
        {
            return VOICED_STRING_TABLE[2];
        }
        
        return VOICED_STRING_TABLE[2];
    }
    
    
    return VOICED_STRING_TABLE[2];
}



/*
 * Calculate a voiced phoneme based on lpc parameters, azc and hurst (all pre-calculated)
 *
 */
char *calcVoicedPhoneme(float *lpc_parameters, int azc, float hurst, voicedVsUnvoiced_t *preVoice)
{
  
    int j;
    float abssum = 0;

    if(hurst > 0.975)
    {
        if(azc > 15)  // if(hurst < 0.9)
        {
           return VOICED_STRING_TABLE[0];
        }
        else
        {
            for(j=1;j<7;j++)
            {
                abssum = abssum + fabsf(lpc_parameters[j]);
            }
            if(abssum < 2.5)
            {
                return VOICED_STRING_TABLE[2];
            }
            else
            {
                if(azc < 7.5)
                {
                    return VOICED_STRING_TABLE[1];
                }
                else
                {
                    return VOICED_STRING_TABLE[3];
                }
            }
        }
    }
    else
    {
        if(hurst > 0.75)  // 0.6
        {
            if(azc > 20)
            {
                return VOICED_STRING_TABLE[7];
            }
            else
            {
                return VOICED_STRING_TABLE[6];
            }
            
        }
        else
        {
            if(azc > 30)
            {
                return VOICED_STRING_TABLE[5];
            }
            else
            {
                return VOICED_STRING_TABLE[4];
            }
        }
    }
    
    return VOICED_STRING_TABLE[2];
}