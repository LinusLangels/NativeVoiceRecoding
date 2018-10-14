//
//  CalcPhonemeOrderProbability.c
//  MicrophoneRecorder
//
//  Created by John Philipsson on 2016-01-28.
//  Copyright © 2016 John Philipsson. All rights reserved.
//

#include "CalcPhonemeOrderProbability.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>


FILE *debugfile_ptr;

// Vowels
char VOWEL_STRING_TABLE_PROB[10][3] = {"ii\0", "ih\0","E\0","ae\0","u\0","a\0","o\0","uu\0","oo\0","@@\0"};
// these phonemes is unvoiced or similar
char UNVOICED_STRING_TABLE_PROB[5][3] = {"f\0", "k\0","p\0","s\0","t\0"};
// these phonemes is voiced or similar
char VOICED_STRING_TABLE_PROB[8][3] = {"b\0", "l\0","m\0","n\0","d\0","g\0","h\0","r\0"};


PhonemeProb_t* calcPhonOrdProb_init(int phonemeOrderBufferSize)
{
    
    
    //debug
    //debugfile_ptr = fopen("debugfile_phonem.txt","w+");
    
    
    int i,j;

    PhonemeProb_t *phonProb = (PhonemeProb_t *)malloc(sizeof(PhonemeProb_t));
    
    if(phonProb == NULL)
    {
        return NULL;
    }
    
    phonProb->phonemeOrderBufferSize = phonemeOrderBufferSize;
    phonProb->delaylinePhoneBuffer  = (char **)malloc(phonProb->phonemeOrderBufferSize * sizeof(char*));
    
    
    
    for(i=0;i<phonProb->phonemeOrderBufferSize;i++)
    {
        phonProb->delaylinePhoneBuffer[i]  = (char *)malloc(PHONEME_LEN * sizeof(char));
        strcpy(phonProb->delaylinePhoneBuffer[i],"sil\0");
    }
    
    //copy TABLES to the phoneme table
    j=0;
    for(i=0;i<10;i++)
    {
        strcpy(phonProb->PHONEME_TABLE_PROB[j],VOWEL_STRING_TABLE_PROB[i]);
        j++;
    }
    for(i=0;i<5;i++)
    {
        strcpy(phonProb->PHONEME_TABLE_PROB[j],UNVOICED_STRING_TABLE_PROB[i]);
        j++;
    }
    for(i=0;i<8;i++)
    {
        strcpy(phonProb->PHONEME_TABLE_PROB[j],VOICED_STRING_TABLE_PROB[i]);
        j++;
    }
    
    phonProb->index_compared_ptr = 0;
    
    return phonProb;
    
}


void calcPhonOrdProb_destroy(PhonemeProb_t *phonProb)
{
    int i;
    
    for(i=0;i<phonProb->phonemeOrderBufferSize;i++)
    {
        free(phonProb->delaylinePhoneBuffer[i]);
    }
    
    free(phonProb->delaylinePhoneBuffer);
    free(phonProb);
    
    
    //fclose(debugfile_ptr);
     
}



void calcPhonOrdProbExec(PhonemeProb_t *phonProb, char *phoneme, int zcr)
{
    
    int i;
    int sum;
    int sil_sum;
    int count_table[23];
    int index_found1,index_found2,index_found3;
    int replaced_phoneme;
    char temp_phoneme[5];
    
    strcpy(temp_phoneme,phoneme);
    
    if(phonProb->index_compared_ptr > (PHONEME_LEN-3))
    {
        phonProb->index_compared_ptr = 0;
    }
    
    
    replaced_phoneme = 0;
    
    for(i=0;i<23;i++)
    {
        count_table[i] = 0;
    }

    for(i=0;i<phonProb->phonemeOrderBufferSize-1;i++)
    {
        strcpy(phonProb->delaylinePhoneBuffer[phonProb->phonemeOrderBufferSize-1-i], phonProb->delaylinePhoneBuffer[phonProb->phonemeOrderBufferSize-2-i]);
    }
    strcpy(phonProb->delaylinePhoneBuffer[0], phoneme);
    
    sum=0;
    sil_sum=0;
    for(int j=0;j<phonProb->phonemeOrderBufferSize;j++)
    {
        for(i=0;i<23;i++)
        {
            if(strcmp(phonProb->delaylinePhoneBuffer[j],"sil\0")==0)
            {
                sil_sum++;
                break;
            }
            if(strcmp(phonProb->delaylinePhoneBuffer[j],phonProb->PHONEME_TABLE_PROB[i])==0)
            {
                sum = sum + i;
                count_table[i] = count_table[i] + 1;
                break;
            }
        }
    }

 
    //get first max from count table
    int max_index = -1;
    int max_value = -1;
    for(i=0;i<23;i++)
    {
        if(count_table[i] > max_value)
        {
            max_value = count_table[i];
            max_index = i;
        }
    }
    
    
    // This one replaces phoneme with the the max found phoneme during the 5 latest phonemes (incl. sil)
    /*
    if(sil_sum > sum)
    {
        strcpy(phoneme,"sil\0");
    }
    else
    {
        strcpy(phoneme,phonProb->PHONEME_TABLE_PROB[max_index]);
    }
    */

    
    index_found1 = -1;
    //find index of phoneme

    for(i=0;i<23;i++)
    {
        if(strcmp(phonProb->delaylinePhoneBuffer[0],phonProb->PHONEME_TABLE_PROB[i])==0)
        {
            index_found1 = i;
            break;
        }
    }
    
    //check middle phoneme
    index_found3 = -1;
    for(i=0;i<23;i++)
    {
        if(strcmp(phonProb->delaylinePhoneBuffer[1],phonProb->PHONEME_TABLE_PROB[i])==0)
        {
            index_found3 = i;
            break;
        }
    }
    
    
    //if a consonant, then check if this one also have come in last last
    index_found2 = -1;
    if(index_found1 > 9)
    {
        for(i=0;i<23;i++)
        {
            if(strcmp(phonProb->delaylinePhoneBuffer[2+phonProb->index_compared_ptr],phonProb->PHONEME_TABLE_PROB[i])==0)
            {
                index_found2 = i;
                break;
            }
        }
        
    }
    else{
        phonProb->index_compared_ptr = 0;
    }
    
    if(strcmp(phoneme,"sil\0") != 0)
    {
        if((index_found1 > 9) && (index_found2 > 9) && (index_found3 < 10))
        {
            if(strcmp(phonProb->delaylinePhoneBuffer[1],"sil\0") != 0)
            {
                strcpy(phoneme,phonProb->delaylinePhoneBuffer[1]);
                strcpy(phonProb->delaylinePhoneBuffer[0],phonProb->delaylinePhoneBuffer[1]); //replace phonem also in delayline
                replaced_phoneme=1;
                phonProb->index_compared_ptr = phonProb->index_compared_ptr + 1;
            }
            
        }
    }


    
     
    //debug prints
    //fprintf(debugfile_ptr,"ZCR: %d\n",zcr);
    //fprintf(debugfile_ptr,"Phoneme: %s\n",phoneme);

    /*
    fprintf(debugfile_ptr,"====== DELAYLINE PHOENME ========\n");
    for(i=0;i<phonProb->phonemeOrderBufferSize;i++)
    {
        fprintf(debugfile_ptr,"%s\n",phonProb->delaylinePhoneBuffer[i]);
    }
    fprintf(debugfile_ptr,"====== BUT SHOULD HAVE BEEN, IF REPLACED - DELAYLINE PHOENME========\n");
    fprintf(debugfile_ptr,"%s\n",phoneme);
    for(i=1;i<phonProb->phonemeOrderBufferSize;i++)
    {
        fprintf(debugfile_ptr,"%s\n",phonProb->delaylinePhoneBuffer[i]);
    }
    if(replaced_phoneme==1)
    {
        fprintf(debugfile_ptr,"**********REPLACED PHOENME*********************************************************************************************************************\n");
        fprintf(debugfile_ptr,"From %s\n",temp_phoneme);
        fprintf(debugfile_ptr,"To %s\n",phoneme);
       
    }
     */
    

    
}