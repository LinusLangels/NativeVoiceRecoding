//
//  Vowel_Identification.c
//  Plotagon-RecordVoice
//
//  Created by John Philipsson on 2015-10-01.
//  Copyright (c) 2015 John Philipsson. All rights reserved.
//

#include "Vowel_Identification.h"
#include <stdlib.h>
#include <string.h>

const int vowelTableRows = 10;
const int VOWEL_TABLE[10][3] =
   {{270, 2290, 3010},
    {390, 1990, 2550},
    {530, 1840, 2480},
    {660, 1720, 2410},
    {520, 1190, 2390},
    {730, 1090, 2440},
    {570, 840 , 2410},
    {440, 1020, 2240},
    {300, 870 , 2240},
    {490, 1350, 1690}};


// these vowel phonemes are mapped to the freqz in VOWEL_TABLE
char VOWEL_STRING_TABLE[10][3] = {"ii\0", "ih\0","E\0","ae\0","u\0","a\0","o\0","uu\0","oo\0","@@\0"};



vowel_t* vowelIdentification_init(int fs, int framesize)
{
    int i,j;
    vowel_t *vow = (vowel_t *)malloc(sizeof(vowel_t));
    
    if(vow == NULL)
    {
        return NULL;
    }
    
    vow->fs = fs;
    vow->rtld_no_coeff = 13;
    
    for(i=0;i<vow->rtld_no_coeff;i++)
    {
        vow->rtld[i] = 0;
        vow->delay_line_rtld[i] = 0;
        vow->ar_coeff[i] = 0;
        vow->ar_coeff_out[i] = 0;
    }
    vow->ar_coeff[0] = 1;
    vow->ar_coeff_out[0] = 1;
    vow->AR_Energy = 0;
    vow->framesize = framesize;
    
    for(i=0;i<vow->rtld_no_coeff-1;i++)
    {
        for(j=0;j<vow->rtld_no_coeff-1;j++)
            vow->a[i][j] = 0.0;
    }
    
    vow->vowelExists = false;
    vow->vowelString = "\0";
    
    return vow;
}


void vowelIdentification_destroy(vowel_t * vow)
{
    
    free(vow);
    
}



vowel_t* vowelIdentification_exec(vowel_t * vow, float *framebuffer, int fs)
{
    autocorrolation(vow,vow->framesize,framebuffer,vow->rtld);
    vow->AR_Energy = levinson_durbin_recursion(vow,vow->rtld,vow->ar_coeff_out);
    transformPolynomToHessMatrix(vow->ar_coeff_out,vow->rtld_no_coeff-1,vow->a);
    int status = eigOfHessMatrix(vow->a,vow->rtld_no_coeff-1,vow->wr,vow->wi);
    
    if(status == 1)
    {
        vow->vowelExists = false;
        vow->vowelString = "\0";
        return vow;
    }
    
    vow->numberOfFreqz = CalculteFreqz(vow->rtld_no_coeff-1,vow->wr,vow->wi,fs);
    
    int vowelIndex = transformFreqzToVowelIndex(vow->numberOfFreqz,vow->wr);
    if(vowelIndex == -1)
    {
        vow->vowelExists = false;
        vow->vowelString = "\0";
        return vow;
    }
    
    vow->vowelString = transformVowelIndexToVowelString(vowelIndex);
    vow->vowelExists = true;
    return vow;
}







/*
 * Levinson_Durbin recursion, input autocorrelation vector (rtld), vector must
 * be adjusted (no-mirror data), output coeffs in (ar_coeff_out)
 *
 */
float levinson_durbin_recursion(vowel_t * vow,float * rtld, float * ar_coeff_out)
{
    
    int i,j;
    float tmp;
    float k;
    
    float a_old[13];
    
    // reset ar_coeffs every call
    for(i = 0; i < vow->rtld_no_coeff; i++)
    {
        vow->ar_coeff[i] = 0.0;
        ar_coeff_out[i] = 0.0;
    }
    vow->ar_coeff[0] = 1;
    ar_coeff_out[0] = 1;
    
    vow->AR_Energy = rtld[0];
    
    for(i=1;i<vow->rtld_no_coeff;i++)
    {
        tmp=0;
        for(j=1;j<(i);j++)
        {
            tmp=tmp+vow->ar_coeff[j]*rtld[i-j];
        }
        k=(rtld[i]-tmp)/vow->AR_Energy;
        vow->ar_coeff[i]=k;
        if(i>1)
        {
            for(j=1;j<(i);j++)
            {
                vow->ar_coeff[j]=vow->ar_coeff[j]-k*a_old[i-j];
            }
        }
        
        vow->AR_Energy=vow->AR_Energy*(1-(k*k));
        for(j=0;j<vow->rtld_no_coeff;j++)
        {
            a_old[j]=vow->ar_coeff[j];
        }
    }
    for(j=1;j<vow->rtld_no_coeff;j++)
    {
        ar_coeff_out[j]=-1*vow->ar_coeff[j];
    }
    
    return vow->AR_Energy;
    
    
}

/*
 * Autocorrelation function, using sliding-windowing update
 * input audio frame (frame_in) of size (framelength)
 * output auto-correlation vector (rtld)
 *
 */
void autocorrolation(vowel_t * vow, int framelength, float * frame_in, float * rtld)
{
    int i,j;
    
    // reset autocorrolation coeff every call
    for (i = 0;i < vow->rtld_no_coeff; i++)
    {
        rtld[i]=0;
        vow->delay_line_rtld[i]=0;
    }
    
    // shift the delay line one step to the right and add the sample first
    for(j=0;j<framelength;j++)
    {
        for (i = vow->rtld_no_coeff-1; i > 0 ; i--)
        {
            vow->delay_line_rtld[i] = vow->delay_line_rtld[i-1];
        }
        vow->delay_line_rtld[0] = frame_in[j];
        
        for (i = 0;i < vow->rtld_no_coeff; i++)
        {
            rtld[i]=rtld[i] + vow->delay_line_rtld[0]*vow->delay_line_rtld[i];
        }
    }
}

/*
 * transform polynom to an "Upper Hessenberger" matrix.
 * and normalize ploynom at same time
 *
 * input polynom of order n, return matrix (a)
 *
 **/
void transformPolynomToHessMatrix(float *polynom, int n, float a[12][12])
{
    float sign = -1;
    int i,j;
    
    //reset matrix
    for(i=0;i<n;i++)
        for(j=0;j<n;j++)
         a[i][j] = 0.0;
    
    
    for(i=1;i<n;i++)
    {
        a[i][i-1] = 1.0;
    }
    for(i=0;i<n;i++)
    {
        a[0][i] = polynom[i+1]*sign;
    }
}




/*
 * input a square matrix (a) of size n, returns the complex eigenvalues in wr and wi
 * for matrixes *ONLY* of type, "Upper Hessenberger".
 *
 **/
int eigOfHessMatrix(float a[12][12], int n, float wr[12], float wi[12])
{
    
    int nn,m,l,k,j,its,i,mmin;
    float z,y,x,w,v,u,t,s,r,q,p,anorm;
    anorm = 0.0;
    
    p=q=r=0;
    
    for(i=1;i<=n;i++)
    {
        for(j=IMAX(i-1,1);j<=n;j++)
        {
            anorm += fabs(a[i-1][j-1]);
        }
    }
    
    nn=n;
    t=0.0;
    while (nn>= 1)
    {
        its=0;
        do{
            for(l=nn;l>=2;l--)
            {
                s=fabs(a[l-2][l-2]) + fabs(a[l-1][l-1]);
                if(s == 0.0)
                    s=anorm;
                if((float) (fabs(a[l-1][l-2]) + s) == s)
                    break;
            }
            
            x=a[nn-1][nn-1];
            if(l == nn)
            {
                wr[nn-1]=x+t;
                wi[nn--]=0.0;
            }
            else
            {
                y=a[nn-2][nn-2];
                w=a[nn-1][nn-2]*a[nn-2][nn-1];
                if(l == (nn-1))
                {
                    p=0.5*(y-x);
                    q=p*p+w;
                    z=sqrt(fabs(q));
                    x += t;
                    if(q >= 0.0)
                    {
                        z=p+SIGN(z,p);
                        wr[nn-2]=wr[nn-1]=x+z;
                        if(z)
                            wr[nn-1]=x-w/z;
                        wi[nn-2]=wi[nn-1]=0.0;
                        
                    }
                    else{
                        wr[nn-2]=wr[nn-1]=x+p;
                        wi[nn-2]=-(wi[nn-1]=z);
                    }
                    nn -= 2;
                }else
                {
                    if(its == 30)
                    {
                        //Too many iteration: Abort
                        return 1;
                    }
                    if(its == 10 || its == 20)
                    {
                        t += x;
                        for(i=1;i<=nn;i++)
                            a[i-1][i-1] -= x;
                        s=fabs(a[nn-1][nn-2]) + fabs(a[nn-2][nn-3]);
                        y=x=0.75*s;
                        w=-0.4375*s*s;
                        
                    }
                    ++its;
                    for(m=(nn-2);m>=1;m--)
                    {
                        z=a[m-1][m-1];
                        r=x-z;
                        s=y-z;
                        p=(r*s-w)/a[m][m-1]+a[m-1][m];
                        q=a[m][m]-z-r-s;
                        r=a[m+1][m];
                        s=fabs(p)+fabs(q)+fabs(r);
                        p /= s;
                        q /= s;
                        r /= s;
                        if(m==1)
                            break;
                        u=fabs(a[m-1][m-2])*(fabs(q)+fabs(r));
                        v=fabs(p)*(fabs(a[m-2][m-2])+fabs(z)+fabs(a[m][m])); 
                        if((float) (u+v) == v)
                            break;
                        
                    }
                    for(i=m+2;i<=nn;i++)
                    {
                        a[i-1][i-3] = 0.0;
                        if(i != (m+2))
                            a[i-1][i-4] = 0.0;
                    }
                    for(k=m;k<=nn-1;k++)
                    {
                        if(k != m)
                        {
                            p=a[k-1][k-2];
                            q=a[k][k-2];
                            r=0.0;
                            if(k != (nn-1))
                                r=a[k+1][k-2];
                            if((x=fabs(p) + fabs(r)) != 0.0)
                            {
                                p /= x;
                                q /= x;
                                r /= x;
                            }
                        }
                        if((s=SIGN(sqrt(p*p+q*q+r*r),p)) != 0.0)
                        {
                            if(k == m)
                            {
                                if(l != m)
                                    a[k-1][k-2] = -a[k-1][k-2];
                            }
                            else
                            {
                                a[k-1][k-2] = -s*x;
                            }
                            p += s;
                            x=p/s;
                            y=q/s;
                            z=r/s;
                            q /= p;
                            r /= p;
                            for(j=k;j<=nn;j++)
                            {
                                p=a[k-1][j-1]+q*a[k][j-1];
                                if(k != (nn-1))
                                {
                                    p += r*a[k+1][j-1];
                                    a[k+1][j-1] -= p*z;
                                }
                                a[k][j-1] -= p*y;
                                a[k-1][j-1] -= p*x;
                            }
                            mmin = nn<k+3 ? nn:k+3;
                            for(i=l;i<=mmin;i++)
                            {
                                p=x*a[i-1][k-1]+y*a[i-1][k];
                                if(k != (nn-1))
                                {
                                    p += z*a[i-1][k+1];
                                    a[i-1][k+1] -= p*r;
                                }
                                a[i-1][k] -= p*q;
                                a[i-1][k-1] -= p;
                            }
                        }
                    }
                }
            }
        }while(l < nn-1);
    }
    
    return 0;
}

/*
 * input the complex eigenvalues in wr and wi, returns the sorted freq bins (three lowest freqz) in wr,
 * and the number of freqz found, if less than three.
 *
 **/
int CalculteFreqz(int n, float wr[], float wi[],int fs)
{
    
    int numberOfFreqz,numberOfSortedFreqz;
    int i,j;
    float freqArray[13];
    
    /*
    for(i=0;i<n;i++)
    {
        printf("wi: %f\n" , wi[i]);
    }
     */
    
    numberOfFreqz=0;
    numberOfSortedFreqz = 0;

	if (n > 13)
		n = 13;
    
    for(i=0;i<n;i++)
    {
        freqArray[i] = 100000; //init to out-of-range in freq-spectra
        if(wi[i] > 0.01)
        {
            freqArray[numberOfFreqz] = (fs*atan2(wi[i],wr[i]))/(2*PI);
            numberOfFreqz++;
        }
    }
    //sort freqz up to the three lowest freq bins
    for(j=0;j<numberOfFreqz;j++)
    {
        float mmin = 100000;
        int mmin_index = 0;
        for(i=0;i<numberOfFreqz;i++)
        {
            if(freqArray[i] < mmin)
            {
                mmin = freqArray[i];
                mmin_index = i;
            }
        }
        wr[numberOfSortedFreqz] = freqArray[mmin_index];
        numberOfSortedFreqz++;
        freqArray[mmin_index] = 100000;
    }
    return MIN(3,numberOfFreqz);
}




/*
 * input the first 3 freqz, return -1 if less than 3 freqz exists
 * otherwise, return freq index to map vowel
 *
 **/
int transformFreqzToVowelIndex(int n, float wr[])
{
    
    int i;
    float diff, minDiff;
    
 
    minDiff = 1000000;
    
    int vowelIndex = -1;
    
    if(n<3)
        return vowelIndex;
    
    for(i=0;i<vowelTableRows;i++)
    {
        if((wr[0] > 0.85*VOWEL_TABLE[i][0]) && (wr[0] < 1.15*VOWEL_TABLE[i][0]) &&  (wr[1] > 0.8*VOWEL_TABLE[i][1]) && (wr[1] < 1.2*VOWEL_TABLE[i][1]) && (wr[2] > 0.7*VOWEL_TABLE[i][2]) && (wr[2] < 1.3*VOWEL_TABLE[i][2]))
        {
            diff = ((wr[0]-VOWEL_TABLE[i][0])*(wr[0]-VOWEL_TABLE[i][0])) + ((wr[1]-VOWEL_TABLE[i][1])*(wr[1]-VOWEL_TABLE[i][1])) + ((wr[2]-VOWEL_TABLE[i][2])*(wr[2]-VOWEL_TABLE[i][2]));
            
            diff = sqrtf(diff);
            if(diff < minDiff)
            {
                vowelIndex = i;
            }
        }
    }

    return vowelIndex;
}

char *transformVowelIndexToVowelString(int vowelIndex)
{

    return VOWEL_STRING_TABLE[vowelIndex];

}

