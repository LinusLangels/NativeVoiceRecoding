//
//  Vowel_Identification.h
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-10-01.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//

#ifndef __Plotagon_RecordVoice__Vowel_Identification__
#define __Plotagon_RecordVoice__Vowel_Identification__

#include <stdio.h>
#include <math.h>
#include <stdbool.h>


#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
static int imaxarg1,imaxarg2;
#define IMAX(a,b) (imaxarg1=(a),imaxarg2=(b),(imaxarg1) > (imaxarg2) ? (imaxarg1) : (imaxarg2))
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
#define MAX(a,b) (a) > (b) ? (a) : (b)
#define MIN(a,b) (a) < (b) ? (a) : (b)
static float PI=3.14159265358979f;



typedef struct vowel_st {
    
    float rtld[13];
    int rtld_no_coeff; // rtld_no_coeff = 13;
    float delay_line_rtld[13];
    float ar_coeff[13];
    float ar_coeff_out[13];
    float AR_Energy;
    int fs;
    int framesize;
    float a[12][12]; // a should be size of rtld_no_coeff-1
    float wr[12]; // a should be length of rtld_no_coeff-1
    float wi[12]; // a should be length of rtld_no_coeff-1

    bool vowelExists;
    int numberOfFreqz;
    char *vowelString;
    
}vowel_t;

int eigOfHessMatrix(float a[12][12], int n, float wr[12], float wi[12]);
int CalculteFreqz(int n, float wr[], float wi[],int fs);
float levinson_durbin_recursion(vowel_t * vow,float * rtld, float * ar_coeff_out);
void autocorrolation(vowel_t * vow, int framelength, float * frame_in, float * rtld);
vowel_t* vowelIdentification_init(int fs, int framesize);
void vowelIdentification_destroy(vowel_t * vow);
vowel_t* vowelIdentification_exec(vowel_t * vow, float *framebuffer, int fs);
void transformPolynomToHessMatrix(float *polynom, int n, float a[12][12]);
int transformFreqzToVowelIndex(int n, float wr[]);
char *transformVowelIndexToVowelString(int vowelIndex);

#endif /* defined(__Plotagon_RecordVoice__Vowel_Identification__) */
